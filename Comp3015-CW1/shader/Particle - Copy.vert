#version 460
const float PI = 3.14159265359;

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexVelocity;
layout (location = 2) in float VertexAge;

out vec3 Position;
out vec3 Velocity;
out float Age;

uniform int Pass;
uniform int smoke = 0;

out float Transp;
out vec2 TexCoord;

uniform float Time;
uniform float DeltaTime;
uniform vec3 Accel;
uniform float ParticleLifetime;
uniform vec3 Emitter = vec3(0.0);
uniform mat3 EmitterBasis;
uniform float ParticleSize;


uniform mat4 ModelViewMatrix;
uniform mat4 Proj;

uniform sampler1D RandomTex;

const vec3 offsets[] = vec3[](vec3(-0.5f,-0.5f,0.0f),vec3(0.5f,-0.5f,0.0f),vec3(0.5f,0.5f,0.0f),
							 vec3(-0.5f,-0.5f,0.0f),vec3(0.5f,0.5f,0.0f),vec3(-0.5f,0.5f,0.0f));

const vec2 texCoords[] = vec2[](vec2(0,0),vec2(1,0),vec2(1,1),vec2(0,0),vec2(1,1),vec2(0,1));

vec3 randomInitialVelocity(){
	float velocity = mix(0.1, 0.5 , texelFetch(RandomTex, 2 * gl_VertexID, 0).r );
	return EmitterBasis * vec3(0, velocity, 0);
}

vec3 randomInitialPosition(){
	float offset = mix(0.0, 0.15 , texelFetch(RandomTex, 2 * gl_VertexID + 1, 0).r );
	return Emitter + vec3(offset, 0, 0);
}


void update(){
	Age = VertexAge + DeltaTime;
	if( VertexAge < 0 || VertexAge > ParticleLifetime){
		Position = randomInitialPosition() + vec3 (1.0,0.0,0.0);
		Velocity = randomInitialVelocity();
		if(VertexAge > ParticleLifetime) 
		Age = (VertexAge - ParticleLifetime) + DeltaTime;
	} else {
		Position = VertexPosition + VertexVelocity * DeltaTime;
		Velocity = VertexVelocity + Accel * DeltaTime;
	}
}

void firerender(){
	Transp = 0.0;
	vec3 poscam = vec3(0.0);
	if(VertexAge >= 0.0){
		poscam = (ModelViewMatrix * vec4(VertexPosition, 1)).xyz + offsets[gl_VertexID] * ParticleSize;
		Transp = clamp(1.0 - VertexAge / ParticleLifetime, 0, 1);
	}
	TexCoord = texCoords[gl_VertexID];

	gl_Position = Proj * vec4(poscam, 1);
}

void smokerender(){
	Transp = 0.0;
	vec3 poscam = vec3(0.0);
	if(VertexAge >= 0.0){
	 float agePct = VertexAge/ParticleLifetime;
		poscam = (ModelViewMatrix * vec4(VertexPosition, 1)).xyz + offsets[gl_VertexID] * mix(0.1, 2.5,agePct);
		Transp = clamp(1.0 - agePct, 0, 1);
	}
	TexCoord = texCoords[gl_VertexID];

	gl_Position = Proj * vec4(poscam, 1);
}

void main(){

	if(Pass ==1)
		update();
	else if(smoke ==1){
		firerender();
	}else{
		smokerender();
    }

}
