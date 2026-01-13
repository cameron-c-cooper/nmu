========================
Traffic Generation TODOs
========================
----------
Objectives
----------
So the primary objective moving forward with traffic generation is to create
real life scenarios that may be applied to test the network monitor. This will
require packets of different types and different models. Implementing full 
traffic generation need some different models depending on what the goal is 
for the traffic generation, so I will likely be switching back and forth on how
often packets are generated.

From here, it may be deduced that the best case scenario is to use bash scripts
in order to generate the topology. However, in my opinion, it may be worth it
to use python in order to create the traffic. It would help manage it better,
but because topology would be a one and done thing then bash would work best for
the topology.

---------
Resources
---------
* https://en.wikipedia.org/wiki/Traffic_generation_model
* https://en.wikipedia.org/wiki/Greedy_source 
* https://en.wikipedia.org/wiki/Long-tail_traffic
* https://en.wikipedia.org/wiki/Poisson_point_process
* https://en.wikipedia.org/wiki/Pareto_distribution
