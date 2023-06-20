# Procedural Terrian Generation
This project implements procedural terriain generation mainly using `C++` on `Unreal Engine 5.2`. 

Currently, I use 2D Perlin Noise to generate Z values for the terrain and I am able to procedural generate a pre-defined sized landmass. I've assigned different vertex colours at the different height values to represent different regions like water, grass and mountain. Moving forward, I would like to replace Perlin Noise to multi-layered Fractional Browning Motion for more dynamic height data, implement infinite terrain generation that generates terrain chunks as the user moves through the world and generate more realistic terrain.

![ue5-procedural-terrain-generation](https://github.com/XxAdi101xX/Procedural-Terrain-Generation/assets/18451835/043290dd-b7d9-4c30-80a4-ddfea457f867)

## Credits
Henrik Kniberg's [youtube video](https://www.youtube.com/watch?v=CSa5O6knuwI) on Minecraft terrain generation has been pivotal in explaining the core concepts behind terrain generation as a whole. I've also been following through Sebastian Lague's [excellent youtube series](https://www.youtube.com/watch?v=wbpMiKiSKm8&list=PLFt_AvWsXl0eBW2EiBtl_sxmDtSgZBxB3) on procedural terrain generation as I'm developing my own generator.
