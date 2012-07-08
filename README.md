OSMGarminPortal
===============

server management for garmin tile generation out of OSM data 

what this program should do:

- take a static area.list file from a splitter output to define tile boundaries [done]
- take a shape file for a chooseable countries/other areas [done]
- match the selected country to a fixed set of tiles [wip]
- filter the set further due to contents of a cache and a timeout setting [planned]
- start spitter.jar on just the selected, non-cached tiles for a osm data file (.pbs) [planned]
- combine the generated tiles to a gmapsupp.img [planned]

- export all path/option/workflow settings of the external tools to a central settings file [planned]
- make several, choosable profiles available through the settings file [planned]
