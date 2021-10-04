#include <stdio.h>
#include <string.h>
#include <math.h>
#include "env.h"

///////////////////////////////////

float distSq(float x1, float y1, float z1, float x2, float y2, float z2)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	float dz = z2 - z1;
	
	return dx*dx + dy*dy + dz*dz;
}

uint64_t wyhash64_x; 
uint64_t randI()
{
  wyhash64_x += 0x60bee2bee120fc15;
  __uint128_t tmp;
  tmp = (__uint128_t) wyhash64_x * 0xa3b195354a39b70d;
  uint64_t m1 = (tmp >> 64) ^ tmp;
  tmp = (__uint128_t)m1 * 0x1b03738712fad5c9;
  uint64_t m2 = (tmp >> 64) ^ tmp;
  return m2;
}

float randF()
{
	return randI() / (float)1.8446744E19;
}

float randFRange(float min, float max){
	return min + (max-min)*randF();
}

///////////////////////////////////

uint8_t img[WIDTH*HEIGHT*3];

static int ticks = 0;

///////////////////////////////////

typedef struct
{
	union
	{
	float x;
	float r;
	};
	
	union
	{
	float y;
	float g;
	};
	
	union
	{
	float z;
	float b;
	};
} vec3;

void vec3_set(vec3 *v, float x, float y, float z)
{
	v->x = x;
	v->y = y;
	v->z = z;
}

float vec3_distSq(vec3 v1, vec3 v2)
{
	return distSq(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z);
}

float vec3_magnitudeSq(vec3 v)
{
	return distSq(0, 0, 0, v.x, v.y, v.z);
}

vec3 vec3_scale(vec3 v, float scalar)
{
	return (vec3) { scalar * v.x, scalar * v.y, scalar * v.z };
}

vec3 vec3_normalize(vec3 v)
{
	return vec3_scale(v, 1/sqrt(vec3_magnitudeSq(v)));
}

vec3 vec3_add(vec3 v1, vec3 v2){
	vec3 sum;
	
	sum.x = v1.x + v2.x;
	sum.y = v1.y + v2.y;
	sum.z = v1.z + v2.z;
	
	return sum;
}

float vec3_dot(vec3 v1, vec3 v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

float vec3_ang(vec3 v1, vec3 v2)
{
	float dot = vec3_dot(v1, v2);
	float mag = sqrt(vec3_magnitudeSq(v1)*vec3_magnitudeSq(v2));
	
	return acos(dot/mag);
}

struct
{
	vec3 pos;
	vec3 dir;
} camera;

typedef struct
{
	vec3 pos;
	vec3 col;
	float r;
	float yVel;
} sphere;

float sphere_vec3_dist(sphere s, vec3 v)
{
	return sqrt(vec3_distSq(s.pos, v)) - s.r;
}

sphere scene[2];

sphere *closest;

float scene_dist(vec3 pos)
{
	float minDist = 100000;
	closest = scene;
	
	float newDist;

	for(int i = 0; i < sizeof(scene) / sizeof(sphere); ++i)
	{
		newDist = sphere_vec3_dist(scene[i], pos);
		if(newDist < minDist)
		{
			minDist = newDist;
			closest = scene + i;
		}
	}

	newDist = pos.y+5;
	if(newDist < minDist)
	{
			minDist = newDist;
	}

	return minDist;
}

vec3 cast_ray(vec3 origin, vec3 direction, int calls, int bounces){
	float stepSize = scene_dist(origin);
	vec3 dest = vec3_add(origin, vec3_scale(direction, stepSize));

	//floor
	if(dest.y < -5) return (fabs(dest.x - floor(dest.x)) < .1f || fabs(dest.z - floor(dest.z)) < .1f ) ?
		(vec3) {1.f, 1.f, 1.f}
		:
		(vec3) {fabs(direction.y/5.f), fabs(direction.y/5.f), fabs(direction.y/5.f)};

	if(/*stepSize > 100 ||*/ vec3_distSq(dest, camera.pos) > 100*100 /*|| calls > 20*/ || bounces > 2) return (vec3) {.25f*direction.y+.7f, .25f*direction.y+.7f /*1 - calls/(float)30.f*/, 1.f}; //sky

	//bounce
	if(stepSize < 0.05)
	{
		vec3 normal = vec3_add(dest, vec3_scale(closest->pos, -1));
		
		vec3 newDirection = normal;
		float scatterAmt = .5f;
		//diffuse scatter:
		//newDirection = vec3_add(newDirection, (vec3){randFRange(-scatterAmt,scatterAmt),randFRange(-scatterAmt,scatterAmt),randFRange(-scatterAmt,scatterAmt)});
		newDirection = vec3_normalize(newDirection);
		
		dest = vec3_add(dest, vec3_scale(newDirection, .1f));

		/////////

		sphere *currentClosest = closest;

		vec3 reflection = cast_ray(dest, newDirection, 0, bounces + 1);

		return vec3_add(
			//vec3_scale(vec3_add(currentClosest->col, vec3_scale(dest, 5)) , .5f),
			vec3_scale(currentClosest->col , .5f),
			vec3_scale(reflection , .5f)
		);
	}
	
	return cast_ray(dest, direction, calls+1, bounces);
}

///////////////////////////////////

void init()
{
	memset(&camera, 0, sizeof(camera));
	camera.dir.z = 1;
	camera.pos.y = -4;

	vec3_set(&(scene[0].pos), 0, -3, 2);
	vec3_set(&(scene[1].pos), -3, -2, 5);

	vec3_set(&(scene[0].col), 1.f, 0.f, 0.f);
	vec3_set(&(scene[1].col), 0.f, 0.f, 1.f);

	scene[0].r = 1;
	scene[1].r = 1;
}

void draw()
{	
	for(int y = 0; y < HEIGHT; ++y)
	{
		for(int x = 0; x < WIDTH; ++x)
		{
			int colIndex = 3*(y*WIDTH + x);
			uint8_t *r = img + colIndex;
			uint8_t *g = img + colIndex + 1;
			uint8_t *b = img + colIndex + 2;
			
			////////////////////////////////////
			
			vec3 finalCol;
			memset(&finalCol, 0, sizeof(vec3));
			
			for(int i = 0; i < NUM_SAMPLES; ++i)
			{
				float offset = 0;
				if(NUM_SAMPLES > 1) offset = randF();
				float tx = (x + offset) / (float)WIDTH;
				tx -= 0.5;
				tx *= 2;
				tx *= WIDTH / (float)HEIGHT;
				
				if(NUM_SAMPLES > 1) offset = randF();
				float ty = (y + offset) / (float)HEIGHT;
				ty -= 0.5;
				ty *= 2;
				
				vec3 pixelRayDir;
				pixelRayDir = camera.dir;
				pixelRayDir.x += tx;
				pixelRayDir.y += ty;
				
				pixelRayDir = vec3_normalize(pixelRayDir);
				
				//vec3 finalCol += cast_ray(camera.pos, pixelRayDir, 0, 0);
				finalCol = vec3_add(finalCol, cast_ray(camera.pos, pixelRayDir, 0, 0));
			}
			
			finalCol = vec3_scale(finalCol, 1/(float)NUM_SAMPLES);
			/*
			float tx = x / (float)WIDTH;
			tx -= 0.5;
			tx *= 2;
			tx *= WIDTH / (float)HEIGHT;
			
			float ty = y / (float)HEIGHT;
			ty -= 0.5;
			ty *= 2;
			
			vec3 pixelRayDir;
			pixelRayDir = camera.dir;
			pixelRayDir.x += tx;
			pixelRayDir.y += ty;
			
			pixelRayDir = vec3_normalize(pixelRayDir);
			
			vec3 finalCol = cast_ray(camera.pos, pixelRayDir, 0, 0);
			*/
			
			*r = 255*finalCol.r;
			*g = 255*finalCol.g;
			*b = 255*finalCol.b;
		}
	}
}

void moveCamera(MoveDir d)
{
	float spd = 0.2;
	switch(d)
	{
		case mcForward: camera.pos.z += spd; break;
		case mcBackward: camera.pos.z -= spd; break;
		case mcLeft: camera.pos.x -= spd; break;
		case mcRight: camera.pos.x += spd; break;
		case mcUp: camera.pos.y += spd; break;
		case mcDown: camera.pos.y -= spd; break;
	}
}

void moveObject(MoveDir d)
{
	vec3 *objPos = &(scene[0].pos);
	float spd = 0.2;
	switch(d)
	{
		case mcForward: objPos->z += spd; break;
		case mcBackward: objPos->z -= spd; break;
		case mcLeft: objPos->x -= spd; break;
		case mcRight: objPos->x += spd; break;
		case mcUp: objPos->y += spd; break;
		case mcDown: objPos->y -= spd; break;
	}
}

void update()
{
	for(int i = 0; i < sizeof(scene) / sizeof(sphere); ++i)
	{
		sphere *s = scene + i;
		s->yVel -= .02f;
		if(s->pos.y - s->r < -5) s->yVel *= -0.8;
		s->pos.y += s->yVel;
	}

	++ticks;
}