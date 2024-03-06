## DMX Notebook

A mobile note-taking plugin for [DMX](https://dmx.berlin/)

### Installation

#### For normal use
Copy 'dmx-notebook.jar' into the bunde-deploy folder of your DMX instance

#### For development
Clone into the bundle-dev/ folder of you DMX instance (or modules-external/ if your DMX is built from source) and run 
```
cd dmx-notebook
mvn clean package
```
to deploy.

### Usage

The mobile interface can be accessed in https://localhost:8080/systems.dmx.notebook/#/

### Credits

Based on Jörg Richter's [DMX Mobile](https://github.com/jri/dm5-mobile) plugin
