# CW1_GLSL
 
Visual Studio 2019 and Operating System windows 10

----------------------------------------------------------
How to use
----------------------------------------------------------

The executable is a Optimised Developer Tool prototype to show off my knowledge of GLSL. 

When the executable is run, with some statures and 2 fire chalasis

this Developer Tool is a simple day cycle with the inpit being bound to the number keys on a keyboard

each number key is bound to a light poaition (time of day) as shown below

![](media/sense.png)

----------------------------------------------------------
How does it work / how does the code fit together
----------------------------------------------------------

when the progeam starts up it compiles all of the shaders that it will need to use
for this Developer Tool 5 shaders are used these shaders are:

shader one (basic_uniform) is used to create the 4 staures and the corrisponding lights
the shader sarts off by creating the texures that will be renderd on the modles for the
floor it is fairly staright forward it just mises the ground texture with a water testre.
but for the staure texture it gets a bit more comlicated as the stature not only uses the
sature and the moss texures moxed to gether but it also uses a generated moise texture 
with a color of green to add a darker looking moss to all of the staures.

the second shader (normal) is ued for the fire chalasis with in the scene. this shader implemnets pbr shading on the modles to give them a more realistic teture and light mapping.
it does this by calulating microfacet in the serface of the modlesand uses these in approximating the reflections of the lights. it also takes in a normal mapped teture and miex this into the texture of the model.

shade 3 (skybox) as the name suggests this shader is used to create a skybox arround the scene and it does this by taking in 6 textures and then cube mapping them together.

shaders 4 and 5 are almost identical. these shaders are used to create the partical effect for the fire and smoke within the scene. for this to be done they instantiate a partical and asighn it a velocity age direction and start position. once the partical is emmited from its position its collore slobble becomes darker and it starts to fade the longer it has been alove until it disapers when it hits its life time.

--------------------------------------------------------------------------------------------
What makes your shader program special and how does it compare to similar things?
--------------------------------------------------------------------------------------------

this project started with the prototype used in glsl_cw1 in which it used aq modle you could
move around withn wasd and had 3 lights and a spotlight (which was not implemented correctaly and therefor emmited no light).

this development tool prototype is diffrent to similer onces due to the light positions being binded to the number keys and nor updated through the update() function. this was done so as to beable to get a better view of what the light was doing at that postiion and not ha it contanly moving.

also this shader is special due to the partical shader also being bound to the keys so when the light is lower the fire starts up but when it is high in the sky the fire is put out and smoke starts billowing out instead.

