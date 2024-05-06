## With cache

```Running 10s test @ http://localhost:8081  
12 threads and 512 connections  
Thread Stats   Avg      Stdev     Max   +/- Stdev  
  Latency     2.47ms    3.19ms 173.72ms   96.13%  
  Req/Sec     6.05k     1.02k    6.72k    86.14%  
Latency Distribution  
   50%    2.28ms  
   75%    2.75ms  
   90%    4.19ms  
   99%    8.28ms  
60774 requests in 10.10s, 16.63MB read  
Socket errors: connect 0, read 266651, write 0, timeout 0  
Requests/sec:   6017.23  
Transfer/sec:      1.65MBS 
```
## No cache

```Running 10s test @ http://localhost:8081  
12 threads and 512 connections  
Thread Stats   Avg      Stdev     Max   +/- Stdev  
  Latency    21.81ms   85.88ms   1.78s    96.40%  
  Req/Sec   544.12    325.45     1.78k    89.00%  
Latency Distribution  
   50%    3.60ms  
   75%   12.90ms  
   90%   45.65ms  
   99%  212.41ms  
5444 requests in 10.10s, 1.49MB read  
Socket errors: connect 0, read 284185, write 0, timeout 6  
Requests/sec:    539.19  
Transfer/sec:    151.12KB 
```