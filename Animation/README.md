This animation has been made based on my UE5 rendering, I animated camera movements but also objects, lights and character.

As my Unreal project is pretty big I'm still working on making it smaller in order to publish it here, 

To reduce the size I'm thinking of deleting pre existing UE5 files not used in my animation, deleting some objects with too complex geometry and reimporting some
textures with a lower resolution.

I created my animation using rig controls and creating different keys for the movement of my assets.

I had some issues with interpolation not taking the shortest path between two poses, making full turn rotation instead of little ones that I fixed by adding 360 to some angle values. I had issues with the metahuman animations wich where hard to make realistic because of the details of the model and the complexity of the skeleton, especially the fingers.

Some overlapping animations where stoping the animation from being played during the rendering, however it was being played in the sequencer which made me confuse.
It took me a long time to find the problem as it wasn't indicated on UE5, I had to change some setting to allow overlapping animations in order to fix the problem.
For more details about the scene I advise you to read render.md in the Render folder. 

Alteration :
I reduced the size of both of my render by limiting the resolution to 1080p and by compressing them online. I also had to convert them from .avi to .mp4. For example Final.avi rendered in 4K with no compression was around 20 go. The video do not seem to loose to much quality compared to the huge gain in term of size.
