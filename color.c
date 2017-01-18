/*
 *	Copyright 2016 
 *	
 *	This file is part of Veta.
 *	
 *	Veta is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *	
 *	Veta is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *	
 *	You should have received a copy of the GNU General Public License
 *	along with Veta.  If not, see <http://www.gnu.org/licenses/>.
*/
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<assert.h>
#include<time.h>

#ifdef WINDOWS 
int random(){
	return rand();
}
#endif

#include"color.h"

int num_colors,color_initialized=0;;
rgb *colors;
yuv *yuv_colors;

void init_color(int n_colors){
	num_colors=n_colors;
	colors=malloc(n_colors*sizeof(rgb));
	yuv_colors=malloc(n_colors*sizeof(yuv));

	generate_colors(0,0.5);

	color_initialized=1;
}

void destroy_color(){
	free(colors);
	free(yuv_colors);
}

int clamp(int v,int min,int max){
	if(v>max) v=max;
	else if( v < min) v=min;
	return  v;
}

void yuv2rgb(rgb *rgb,yuv yuv){
	rgb->r=clamp(255.0*(yuv.y+C_1*(yuv.v)),0,255);
	rgb->g=clamp(255.0*(yuv.y-C_2*(yuv.u)-C_3*(yuv.v)),0,255);
	rgb->b=clamp(255.0*(yuv.y+C_4*(yuv.u)),0,255);
}

void print_yuv(yuv c){
	printf("{%0.3f %0.3f %0.3f}\n",c.y,c.u,c.v);
}

void print_rgb(rgb c){
	printf("#%02x%02x%02x\n",c.r,c.g,c.b);
}

double random_double(){
	return random()/(double)RAND_MAX;
}

double rand_y(){
	double a=random_double()*0.5;
	return a;
}
double rand_u(){
	return 2*MAX_U*random_double()-MAX_U;
}

double rand_v(){
	return 2*MAX_V*random_double()-MAX_V;
}
void random_yuv(yuv *c){
	c->y=random_double();	
	c->u=rand_u();
	c->v=rand_v();
}

double min_dist(yuv c,int len){
	double dist,min=500000;
	yuv d;
	for(int i=0;i<len;i++){
		d.y=yuv_colors[i].y-c.y;
		d.u=yuv_colors[i].u-c.u;
		d.v=yuv_colors[i].v-c.v;

		dist=sqrt(d.y*d.y+d.u*d.u+d.v*d.v);
		if( dist < min ) min = dist;
	}
	return dist;
}

void randomize_yuv(int static_component,yuv *c){
	if(static_component!=0)c->y=rand_y();
	if(static_component!=1) c->u=rand_u();
	if(static_component!=2) c->v=rand_v();	
}

void generate_colors(int static_component,double dist_treshold){
	yuv c;
	double dist;

	randomize_yuv(-1,&c);

	for(int i=0;i<num_colors;i++){
	// attempt to create a specific contrast between the colors not sure how  well it works
		if(!i ){
			randomize_yuv(static_component,&c);
		}else{
			randomize_yuv(static_component,&c);
			int cnt=0;
			while(dist_treshold > (dist=min_dist(c,i))){
				assert(++cnt < STUCK_TRESHOLD);

				randomize_yuv(static_component,&c);
			} 
		}

		yuv_colors[i].y=c.y;
		yuv_colors[i].u=c.u;
		yuv_colors[i].v=c.v;
	}
	
	// convert the colors to rgb
	for(int i=0;i<num_colors;i++){
		yuv2rgb(&colors[i],yuv_colors[i]);
	}
}

rgb get_color(int n,int bright){
	assert(color_initialized);
	assert(n >= 0 && n < num_colors);
	rgb c;
	yuv c1;

	c1=yuv_colors[n];
	if(bright) c1.y+=0.5;
	yuv2rgb(&c,c1);
	return c;
}
