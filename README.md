![UE 5.6](https://img.shields.io/badge/UE-5.6-darkgreen) 
# PCG Extended Toolkit + Watabou

![PCGEx](https://raw.githubusercontent.com/Nebukam/PCGExtendedToolkit/refs/heads/docs/_sources/smol-logo.png)

## Purpose of Plugin

This **experimental** Watabou plugin allows to convert JSON exports of [Watabou Generators](https://watabou.itch.io/) into PCG & [PCG Extended Toolkit](https://github.com/nebukam/PCGExtendedToolkit) data.  

<img width="1016" height="540" alt="image" src="https://github.com/user-attachments/assets/b9cd1aa8-9f61-49b6-8428-5082f60bc295" />


## EXPERIMENTAL Status

PCGExtendedToolkitWatabou (this plugin) is **experimental** code. Watabou JSON data structure may change over time and it may stop working without me noticing. If that's the case, create an issue and I'll look into the changes; the code is designed to support multiple data versions, starting with the latest at the time of creating this plugin.

## Getting Started

Go to any of the currently supported Watabou generators :
- [Medieval Fantasy City Generator](https://watabou.itch.io/medieval-fantasy-city-generator)
- [Neighborhood Generator](https://watabou.itch.io/neighbourhood)
- [Village Generator](https://watabou.itch.io/village-generator)

Then :  
1. Right-click and export to JSON.  
2. Rename the file extension from `.json` to `.wataboujson` -- _this is key for the auto-import to kick-in in Unreal._  
3. Drag'n drop the file to Unreal, this shoudld generate a new Data Asset. Save it.
4. In a PCG Graph, use the `Load Watabout Data` node and pick your data asset.

> Note that the content of the JSON export is based on what is displayed in the generator. Some generator allow for different type of data to be available such as districts and other things -- play around with them!

For questions & support, join the [PCGEx Discord Server](https://discord.gg/mde2vC5gbE)!

