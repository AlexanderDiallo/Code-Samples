# Rhythm Game in Unreal C++
 
The following code sample contains the main elements of the C++ framework I created to support the rhythm based gameplay for a school project as can be seen on my [portfolio website](https://alexdiallo.com/portfolio/bongo-beater-unreal-vr-project/).

The elements of the system are described in detail in the [TDD](https://alexdiallo.com/2020/01/22/technical-design-of-a-rhythm-game/) I wrote for the project, but I have summarised the main elements below.

The general system is explained here:

## BongoBeaterGameMode
The class responsible for handling the progression of the gameplay. More specifically, it loads song and note data from a (blueprint) data table which it then uses to handle the playback of the song and fire note spawning events at particular beats (which is then handled by the note spawner). It then handles the events raised by the NoteSpawner for when the player hits (or misses) a note.
## NoteLane
A NoteLane is used to represent the note highway to the player. This class contains a spline (which is modifiable by the designer) and the asteroids (i.e. notes) move along this spline.
## AsteroidSpawner (i.e. NoteSpawner)
Used to handle the spawning of the asteroids. The AsteroidSpawner spawns an asteroid on the right note lane and keeps track of an Asteroid's lifetime. This class is also responsible for removing an asteroid if it is hit at the right time by the player (or missed) at which point it fires a NoteHit event to notify the BongoBeaterGameMode.
## Asteroid (i.e. Note)
A class that is used to visually represent a note to the player. It handles the updating of its position on the note lane it is associated with.