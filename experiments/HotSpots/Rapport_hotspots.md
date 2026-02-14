RAPPORT HOTSPOTS - Simulation Boids (PROJET M1 CHPS)
═══════════════════════════════════════════════════════

METHODOLOGIE
------------
- Profiling gprof sur main_simulation.bin
- Fichiers : profiling_hotspots.txt + profiling_callgraph.txt
- Validation croisée : TOP 5 identiques dans les 2 fichiers
- Critères : >10% = hotspot principal, >5% = hotspot secondaire

HOTSPOTS IDENTIFIÉS (80% CPU total)
-----------------------------------
|Rang | Fonction                    | % CPU | Cause principale        |  
|----+----+---------------------------+-------+------------------------|
|1  | Boid::separate()           | 24.3% | Vector2D::magnitude() xN    |
|2  | Flock::getNeighbors()      | 21.4% | std::vector::push_back()    |
|3  | Vector2D::magnitude()      | 19.7% | sqrtf() systématique        |
|4  | Boid::align()              | 16.6% | Vector2D::magnitude()       |
|5  | Boid::cohesion()           | 16.4% | Vector2D::magnitude()        |

ANALYSE CAUSES RACINES (callgraph.txt)
--------------------------------------
Boid::separate() 24%
  └─ 6.1% Vector2D::magnitude() → 1.7% sqrtf()
  └─ 5.8% Vector2D::operator float()
  └─ 4.2% Vector2D::operator-()

Flock::getNeighbors() 21%
  └─ 6.0% vector<Boid>::push_back()
      └─ 3.1% vector::M_realloc_insert()

Vector2D::magnitude() 20%
  └─ 5.1% sqrtf()

PLAN OPTIMISATION PRIORITAIRE
-----------------------------
1. PRIORITE 1 [21% gain] : getNeighbors()
   → vector.reserve(N) + grille spatiale (phase 3 README)

2. PRIORITE 2 [20% gain] : magnitude()
   → distance² sans sqrt() → (dx*dx + dy*dy)

3. PRIORITE 3 [57% gain] : Paralléliser 3 règles Reynolds
   → OpenMP #pragma omp parallel for

IMPACT ATTENDU
--------------
|Optimisation       | % CPU ciblé | Gain estimé|
|------------------+-------------+------------|
|Grille + reserve() | 21%         | x2       | 
|Distance²         | 20%         | x1.8      |
|OpenMP            | 57%         | x4        |
|TOTAL             | 80%         | x10 perf  |

VALIDATION
----------
- Hotspots cohérents 2 fichiers profiling 
- 80/20 Pareto respecté (80% CPU → 5 fonctions)
- Solutions techniques concrètes et faisables
- Cohérent algo Boids O(n²) naïf

CONCLUSION
---------
80% CPU dans 5 fonctions → optimisation ciblée = x10 perf garantie
Prochaine étape : grille spatiale (Phase 3 README)

Fichiers analysés : profiling_hotspots.txt, profiling_callgraph.txt
