// Architecture MPI :
//   - Rank 0   : calcul DistributedFlock + fenêtre GUI SFML (si USE_SFML)
//                ou master headless synchronisé (si pas de USE_SFML)
//   - Rank 1..N: calcul DistributedFlock uniquement (jamais de GUI)
//
// IMPORTANT : le check if(rank == 0) est la barrière EXTÉRIEURE.
//   #ifdef USE_SFML est à l'intérieur du bloc rank 0 uniquement.
//   Les processus workers n'initialisent jamais SFML, même si USE_SFML est défini.
//
// Chaque step de simulation :
//   1. Rank 0 diffuse les paramètres via MPI_Bcast
//   2. Tous les ranks font updateAllDistributed()
//   3. Tous les ranks participent à gatherAllBoids() (collectif MPI_Gatherv)
//   4. Rank 0 dessine (ou affiche les stats en mode headless)

#include <mpi.h>
#include <Kokkos_Core.hpp>
#include "boid.h"
#include "vector2D.h"
#include "DistributedFlock.h"
#include "MPIManager.h"

#ifdef USE_SFML
#include <SFML/Graphics.hpp>
#include "GUI.h"
#include <cmath>
#include <cstdlib>
#include <sstream>
#endif

const int WIDTH       = 1200;
const int HEIGHT      = 800;
const int TOTAL_BOIDS = 500;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    Kokkos::initialize(argc, argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    {
        // Chaque processus crée son DistributedFlock et peuple son sous-domaine
        simulation::DistributedFlock flock(
            static_cast<float>(WIDTH),
            static_cast<float>(HEIGHT),
            50.f
        );
        flock.populateRandomDistributed(TOTAL_BOIDS);
        flock.setWeights(1.f, 1.f, 1.f);
        flock.setNeighborRadius(50.f);

        // ── Séparation master / workers ──────────────────────────────────────
        // Cette barrière est TOUJOURS active, que USE_SFML soit défini ou non.
        // Seul rank 0 crée une fenêtre (si USE_SFML). Les workers n'y touchent jamais.
        if (rank == 0) {

#ifdef USE_SFML
            // ── Rank 0 avec SFML : boucle GUI ───────────────────────────────
            sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Banc de poissons (distribué)");
            window.setFramerateLimit(60);

            sf::Font font;
            const char* fontPaths[] = {
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
                "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf",
                "/System/Library/Fonts/Helvetica.ttc",
                "/System/Library/Fonts/Courier.ttc",
            };
            bool fontLoaded = false;
            for (const auto& path : fontPaths) {
                if (font.openFromFile(path)) { fontLoaded = true; break; }
            }
            if (!fontLoaded) {
                std::cerr << "[rank 0] Impossible de charger une police\n";
                // Signaler aux workers d'arrêter via Bcast (ils attendent un Bcast, pas un Send)
                float stopParams[5] = {0.f, 0.f, 0.f, 0.f, 0.f};
                int   stopFlag = -1;
                MPI_Bcast(stopParams, 5, MPI_FLOAT, 0, MPI_COMM_WORLD);
                MPI_Bcast(&stopFlag,  1, MPI_INT,   0, MPI_COMM_WORLD);
                std::vector<simulation::Boid> dummy;
                flock.gatherAllBoids(dummy);  // finalise le Gatherv collectif
                Kokkos::finalize();
                MPI_Finalize();
                return -1;
            }

            Slider sepSlider   (font, "Separation",      10.f,  50.f, 150.f, 0.f,   10.f,  1.f);
            Slider aliSlider   (font, "Alignment",       10.f, 100.f, 150.f, 0.f,   10.f,  1.f);
            Slider cohSlider   (font, "Cohesion",        10.f, 150.f, 150.f, 0.f,   10.f,  1.f);
            Slider radiusSlider(font, "Neighbor radius", 10.f, 200.f, 150.f, 20.f, 150.f, 50.f);
            Slider countSlider (font, "Count",           10.f, 250.f, 150.f, 10.f, 1000.f, static_cast<float>(TOTAL_BOIDS));

            sf::RectangleShape uiPanel({200.f, static_cast<float>(HEIGHT)});
            uiPanel.setFillColor(sf::Color(50, 50, 50));
            uiPanel.setPosition({0.f, 0.f});

            sf::RectangleShape applyBtn({80.f, 30.f});
            applyBtn.setPosition({10.f, 310.f});
            applyBtn.setFillColor(sf::Color(100, 150, 100));

            sf::Text applyText(font, "Apply");
            applyText.setCharacterSize(14);
            applyText.setFillColor(sf::Color::White);
            {
                auto b = applyText.getLocalBounds();
                applyText.setPosition({10.f + (80.f - b.size.x) / 2.f - 2.f,
                                       310.f + (30.f - b.size.y) / 2.f - 5.f});
            }

            sf::Text fpsText(font, "");
            fpsText.setCharacterSize(14);
            fpsText.setFillColor(sf::Color::White);
            fpsText.setPosition({8.f, 8.f});

            sf::Text rankText(font, "");
            rankText.setCharacterSize(12);
            rankText.setFillColor(sf::Color(180, 255, 180));
            rankText.setPosition({8.f, 360.f});

            sf::Clock clock;
            int   frameCount  = 0;
            float timeElapsed = 0.f;

            while (window.isOpen()) {
                // ── Événements ──────────────────────────────────────────────
                bool applyPressed = false;
                while (auto evOpt = window.pollEvent()) {
                    const auto& ev = *evOpt;
                    if (ev.is<sf::Event::Closed>()) window.close();
                    if (auto* kp = ev.getIf<sf::Event::KeyPressed>())
                        if (kp->code == sf::Keyboard::Key::Escape) window.close();

                    sepSlider.handleEvent(ev, window);
                    aliSlider.handleEvent(ev, window);
                    cohSlider.handleEvent(ev, window);
                    radiusSlider.handleEvent(ev, window);
                    countSlider.handleEvent(ev, window);

                    if (auto* mp = ev.getIf<sf::Event::MouseButtonPressed>())
                        if (mp->button == sf::Mouse::Button::Left)
                            if (applyBtn.getGlobalBounds().contains(
                                    window.mapPixelToCoords({mp->position.x, mp->position.y})))
                                applyPressed = true;
                }

                // ── Diffuser les paramètres à tous les workers ───────────────
                float params[5] = {
                    sepSlider.getValue(),
                    aliSlider.getValue(),
                    cohSlider.getValue(),
                    radiusSlider.getValue(),
                    countSlider.getValue()
                };
                int applyFlag = applyPressed ? 1 : 0;
                MPI_Bcast(params,     5, MPI_FLOAT, 0, MPI_COMM_WORLD);
                MPI_Bcast(&applyFlag, 1, MPI_INT,   0, MPI_COMM_WORLD);

                flock.setWeights(params[0], params[1], params[2]);
                flock.setNeighborRadius(params[3]);

                if (applyFlag == 1) {
                    // Re-peupler : rank 0 génère tous les boids puis scatter
                    int newCount = static_cast<int>(params[4]);
                    std::vector<simulation::Boid> globalBoids;
                    globalBoids.reserve(newCount);
                    for (int i = 0; i < newCount; ++i) {
                        float x     = static_cast<float>(std::rand() % WIDTH);
                        float y     = static_cast<float>(std::rand() % HEIGHT);
                        float angle = static_cast<float>(std::rand()) / RAND_MAX * 2.f * 3.14159f;
                        simulation::Boid b(x, y);
                        b.velocity = simulation::Vector2D(std::cos(angle) * 50.f, std::sin(angle) * 50.f);
                        globalBoids.push_back(b);
                    }
                    flock.scatterAllBoids(globalBoids);
                }

                // ── Pas de simulation ────────────────────────────────────────
                float dt = clock.restart().asSeconds();
                if (dt > 0.05f) dt = 0.05f;
                flock.updateAllDistributed(dt);

                // ── Collecte tous les boids sur rank 0 ───────────────────────
                std::vector<simulation::Boid> globalBoids;
                flock.gatherAllBoids(globalBoids);

                // ── FPS ──────────────────────────────────────────────────────
                ++frameCount;
                timeElapsed += dt;
                if (timeElapsed >= 1.f) {
                    std::ostringstream oss;
                    oss << "FPS: " << frameCount;
                    fpsText.setString(oss.str());
                    frameCount  = 0;
                    timeElapsed = 0.f;
                }
                {
                    std::ostringstream oss;
                    oss << "Processus MPI: " << size
                        << "\nBoids total: "  << flock.getGlobalBoidCount();
                    rankText.setString(oss.str());
                }

                // ── Rendu ────────────────────────────────────────────────────
                window.clear(sf::Color(20, 20, 20));
                window.draw(uiPanel);
                sepSlider.draw(window);
                aliSlider.draw(window);
                cohSlider.draw(window);
                radiusSlider.draw(window);
                countSlider.draw(window);
                window.draw(applyBtn);
                window.draw(applyText);
                window.draw(fpsText);
                window.draw(rankText);

                for (const auto& b : globalBoids) {
                    sf::ConvexShape fish;
                    fish.setPointCount(3);
                    fish.setPoint(0, { 0.f,   0.f});
                    fish.setPoint(1, {-12.f,  5.f});
                    fish.setPoint(2, {-12.f, -5.f});
                    fish.setFillColor(sf::Color(80, 200, 255));
                    float angle_rad = std::atan2(b.velocity.y, b.velocity.x);
                    fish.setRotation(sf::radians(angle_rad));
                    fish.setPosition({b.position.x, b.position.y});
                    window.draw(fish);
                }
                window.display();
            }

            // ── Signaler aux workers d'arrêter (même protocole Bcast) ────────
            float stopParams[5] = {0.f, 0.f, 0.f, 0.f, 0.f};
            int   stopFlag = -1;  // convention : -1 = arrêt
            MPI_Bcast(stopParams, 5, MPI_FLOAT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&stopFlag,  1, MPI_INT,   0, MPI_COMM_WORLD);
            // Dernière participation au Gatherv collectif
            std::vector<simulation::Boid> dummy;
            flock.gatherAllBoids(dummy);

#else   // USE_SFML non défini : rank 0 joue le rôle de master headless

            // ── Rank 0 sans SFML : master headless synchronisé ───────────────
            // Rank 0 pilote la boucle et maintient le même protocole Bcast/Gather
            // que les workers attendent — indispensable pour les appels MPI collectifs.
            float params[5] = {1.f, 1.f, 1.f, 50.f, static_cast<float>(TOTAL_BOIDS)};
            for (int step = 0; step < 1000; ++step) {
                int applyFlag = 0;
                MPI_Bcast(params,     5, MPI_FLOAT, 0, MPI_COMM_WORLD);
                MPI_Bcast(&applyFlag, 1, MPI_INT,   0, MPI_COMM_WORLD);
                flock.updateAllDistributed(0.016f);
                std::vector<simulation::Boid> allBoids;
                flock.gatherAllBoids(allBoids);
            }
            // Signal d'arrêt aux workers
            float stopParams[5] = {0.f, 0.f, 0.f, 0.f, 0.f};
            int   stopFlag = -1;
            MPI_Bcast(stopParams, 5, MPI_FLOAT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&stopFlag,  1, MPI_INT,   0, MPI_COMM_WORLD);
            std::vector<simulation::Boid> dummy;
            flock.gatherAllBoids(dummy);

            std::cout << "Simulation headless terminée. Boids: "
                      << flock.getGlobalBoidCount() << "\n";
#endif

        } else {
            // ── Rank > 0 : boucle worker ─────────────────────────────────────
            // Identique que USE_SFML soit défini ou non : les workers ne touchent
            // jamais à SFML et obéissent uniquement aux Bcast de rank 0.
            while (true) {
                float params[5];
                int   applyFlag;
                MPI_Bcast(params,     5, MPI_FLOAT, 0, MPI_COMM_WORLD);
                MPI_Bcast(&applyFlag, 1, MPI_INT,   0, MPI_COMM_WORLD);

                if (applyFlag == -1) {
                    // Signal d'arrêt : participer au dernier Gatherv puis quitter
                    std::vector<simulation::Boid> dummy;
                    flock.gatherAllBoids(dummy);
                    break;
                }

                flock.setWeights(params[0], params[1], params[2]);
                flock.setNeighborRadius(params[3]);

                if (applyFlag == 1) {
                    // Rank 0 a fait scatterAllBoids, on reçoit notre part
                    std::vector<simulation::Boid> empty;
                    flock.scatterAllBoids(empty);
                }

                flock.updateAllDistributed(0.016f);

                std::vector<simulation::Boid> dummy;
                flock.gatherAllBoids(dummy);  // contribue au Gatherv collectif
            }
        }
    }

    Kokkos::finalize();
    MPI_Finalize();
    return 0;
}
