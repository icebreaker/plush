#define PLUSH_IMPLEMENTATION
#include "plush.h"

#include <SDL.h>

#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
#endif

#define PLUSH_DEMO_PALETTE_COLORS_MAX 256
#define PLUSH_DEMO_MODELS_MAX 6
#define PLUSH_DEMO_ROTATION_SPEED 0.1f

typedef struct
{
	int w;
	int h;
	uint32_t frames;
	uint32_t fps;
	uint32_t time_last;
	uint32_t time_elapsed;
	uint32_t dt;
	float rotation[3];
	bool rotate;
	pl_Light *light;
	pl_Texture *texture;
	pl_Obj *model;
	pl_Obj *models[PLUSH_DEMO_MODELS_MAX];
	pl_Mat *material;
	pl_Cam *camera;
	pl_uChar palette[PLUSH_DEMO_PALETTE_COLORS_MAX * 3];
	SDL_Window *window;
	SDL_Surface *screen;
	SDL_Surface *buffer;
	SDL_Rect viewport;
} plush_demo_t;

static bool plush_demo_init(
	plush_demo_t *self,
	const char *title,
	const int w,
	const int h,
	const float fov,
	const float aspect,
	const float zoom
);
#ifdef __EMSCRIPTEN__
static void plush_demo_run(void *user);
#else
static void plush_demo_run(plush_demo_t *self);
#endif
static void plush_demo_shutdown(plush_demo_t *self);

int main()
{
	plush_demo_t demo = { 0, };

	if(!plush_demo_init(&demo, "Plush 3D Demo", 1024, 768, 60.0f, (3.0f / 4.0f) * (1024.0f / 768.0f), -8.0f))
	{
		plush_demo_shutdown(&demo);
		return EXIT_FAILURE;
	}

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(plush_demo_run, &demo, -1, 1);
#else
	plush_demo_run(&demo);
	plush_demo_shutdown(&demo);
#endif
	return EXIT_SUCCESS;
}

static bool plush_demo_init(
	plush_demo_t *self,
	const char *title,
	const int w,
	const int h,
	const float fov,
	const float aspect,
	const float zoom
)
{
	SDL_Color colors[PLUSH_DEMO_PALETTE_COLORS_MAX] = { 0, };
	pl_Mat *material;
	int i, j;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SDL_Log("error: failed to initialize SDL: %s", SDL_GetError());
		return false;
	}

	self->window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		w,
		h,
		0
	);
	if(self->window == NULL)
	{
		SDL_Log("error: failed to create window: %s", SDL_GetError());
		return false;
	}

	self->screen = SDL_GetWindowSurface(self->window);
	if(self->screen == NULL)
	{
		SDL_Log("error: failed to get windows surface: %s", SDL_GetError());
		return false;
	}

	self->buffer = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 8, 0, 0, 0, 0);
	if(self->buffer == NULL)
	{
		SDL_Log("error: failed to create back buffer: %s", SDL_GetError());
		return false;
	}

	self->camera = plCamCreate(
		w,
		h,
		aspect,
		fov,
		self->buffer->pixels,
		NULL
    );
	if(self->camera == NULL)
	{
		SDL_Log("error: failed to create camera");
		return false;
	}
	self->camera->Z = zoom;

	self->light = plLightCreate();
	if(self->light == NULL)
	{
		SDL_Log("error: failed to create light");
		return false;
	}
	plLightSet(self->light, PL_LIGHT_VECTOR, 0, 0, 0, 0.6, 1.0);

	material = plMatCreate();
	if(material == NULL)
	{
		SDL_Log("error: failed to create material");
		return false;
	}

	material->NumGradients = 256;
	material->ShadeType = PL_SHADE_FLAT;
	material->PerspectiveCorrect = 1;

	self->texture = plReadPCXTex("data/texture.pcx", 1, 1);

	material->Texture = self->texture;

	if(material->Texture != NULL)
	{
		material->Diffuse[0] = 0;
		material->Diffuse[1] = 0;
		material->Diffuse[2] = 0;
	}

	plMatInit(material);

	plMatMakeOptPal(self->palette, 1, PLUSH_DEMO_PALETTE_COLORS_MAX - 2, &material, 1);

	self->palette[0] = 0;
	self->palette[1] = 0;
	self->palette[2] = 0;

	self->palette[PLUSH_DEMO_PALETTE_COLORS_MAX * 3 - 1] = 255;
	self->palette[PLUSH_DEMO_PALETTE_COLORS_MAX * 3 - 2] = 255;
	self->palette[PLUSH_DEMO_PALETTE_COLORS_MAX * 3 - 3] = 255;

	plMatMapToPal(material, self->palette, 0, PLUSH_DEMO_PALETTE_COLORS_MAX - 1);

	self->material = material;

	for(i = 0, j = 0; i < PLUSH_DEMO_PALETTE_COLORS_MAX; i++, j += 3)
	{
		colors[i].r = self->palette[j + 0];
		colors[i].g = self->palette[j + 1];
		colors[i].b = self->palette[j + 2];
#ifndef __EMSCRIPTEN__
		colors[i].a = 255;
#endif
	}
	SDL_SetPaletteColors(self->buffer->format->palette, colors, 0, PLUSH_DEMO_PALETTE_COLORS_MAX);

	self->models[0] = plRead3DSObj("data/suzanne.3ds", material);
	self->models[1] = plMakeBox(1, 1, 1, material);
	self->models[2] = plMakeTorus(0.5, 1, 8, 8, material);
	self->models[3] = plMakeSphere(1, 8, 8, material);
	self->models[4] = plMakeCone(1, 1, 8, true, material);
	self->models[5] = plMakeCylinder(0.5, 2, 8, true, true, material);

	for(i = 0; i < PLUSH_DEMO_MODELS_MAX; i++)
	{
		if(self->models[i] == NULL)
		{
			SDL_Log("error: failed to create or load model with index %d", i);
			return false;
		}
	}

	self->model = self->models[0];
	self->rotate = true;
	self->rotation[0] = 0;
	self->rotation[1] = PLUSH_DEMO_ROTATION_SPEED;
	self->rotation[2] = 0;

	self->w = w;
	self->h = h;

	self->viewport.x = 0;
	self->viewport.y = 0;
	self->viewport.w = w;
	self->viewport.h = h;

	self->time_last = SDL_GetTicks();
	return true;
}

#ifdef __EMSCRIPTEN__
static void plush_demo_run(void *user)
#else
static void plush_demo_run(plush_demo_t *self)
#endif
{
	SDL_Event e;
	uint32_t time_now;

#ifdef __EMSCRIPTEN__
	plush_demo_t *self = (plush_demo_t *) user;
#endif

#ifndef __EMSCRIPTEN__
	for(bool running = true; running;)
#endif
	{
		while(SDL_PollEvent(&e))
		{
#ifndef __EMSCRIPTEN__
			if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
			{
				running = false;
				continue;
			}
#endif
			if(e.type == SDL_KEYDOWN)
			{
				switch(e.key.keysym.sym)
				{
					case SDLK_1:
					case SDLK_2:
					case SDLK_3:
					case SDLK_4:
					case SDLK_5:
					case SDLK_6:
					{
						self->model = self->models[e.key.keysym.sym - SDLK_1];
						if(self->model == self->models[0])
						{
							self->rotation[0] = 0;
							self->rotation[1] = PLUSH_DEMO_ROTATION_SPEED;
							self->rotation[2] = 0;
						}
						else
						{
							self->rotation[0] = PLUSH_DEMO_ROTATION_SPEED;
							self->rotation[1] = PLUSH_DEMO_ROTATION_SPEED;
							self->rotation[2] = PLUSH_DEMO_ROTATION_SPEED;
						}
					}
					break;

					case SDLK_r:
						self->rotate = !self->rotate;
						break;

					case SDLK_g:
					{
						if(self->material->ShadeType == PL_SHADE_FLAT)
							self->material->ShadeType = PL_SHADE_GOURAUD;
						else
							self->material->ShadeType = PL_SHADE_FLAT;

						plMatInit(self->material);
					}
					break;

					case SDLK_f:
					{
						if(SDL_GetWindowFlags(self->window) & SDL_WINDOW_FULLSCREEN_DESKTOP)
							SDL_SetWindowFullscreen(self->window, 0);
						else
							SDL_SetWindowFullscreen(self->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
					}
					break;
				}

				continue;
			}

			if(e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				self->screen = SDL_GetWindowSurface(self->window);
				SDL_assert(self->screen != NULL);

				SDL_memset(self->screen->pixels, 0, self->screen->h * self->screen->pitch);

				self->viewport.x = (self->screen->w - self->viewport.w) >> 1;
				self->viewport.y = (self->screen->h - self->viewport.h) >> 1;
			}
		}

		if(self->rotate)
		{
			self->model->Xa += self->dt * self->rotation[0];
			self->model->Ya += self->dt * self->rotation[1];
			self->model->Za += self->dt * self->rotation[2];
		}

		SDL_memset(self->buffer->pixels, 0, self->buffer->h * self->buffer->pitch);

		plTextPrintf(self->camera, 0, 0, 0, 255, (pl_sChar *) "FPS: %d", self->fps);
		plTextPrintf(self->camera, 0, self->h - 32, 0, 255, plVersionString);
		plTextPrintf(self->camera, 0, self->h - 16, 0, 255, plCopyrightString);

		plRenderBegin(self->camera);
		plRenderLight(self->light);
		plRenderObj(self->model);
		plRenderEnd();

		SDL_BlitSurface(self->buffer, NULL, self->screen, &self->viewport);
		SDL_UpdateWindowSurface(self->window);

		time_now = SDL_GetTicks();
		self->dt = time_now - self->time_last;
		self->time_last = time_now;

		self->frames++;
		self->time_elapsed += self->dt;
		if(self->time_elapsed >= 1000)
		{
			self->fps = self->frames;
			self->frames = 0;
			self->time_elapsed -= 1000;
		}
	}
}

static void plush_demo_shutdown(plush_demo_t *self)
{
	int i;

	for(i = 0; i < PLUSH_DEMO_MODELS_MAX; i++)
	{
		if(self->models[i] != NULL)
		{
			plObjDelete(self->models[i]);
			self->models[i] = NULL;
		}
	}
	self->model = NULL;

	if(self->texture != NULL)
	{
		plTexDelete(self->texture);
		self->texture = NULL;
	}

	if(self->material != NULL)
	{
		self->material->Texture = NULL;

		plMatDelete(self->material);
		self->material = NULL;
	}

	if(self->light != NULL)
	{
		plLightDelete(self->light);
		self->light = NULL;
	}

	if(self->camera != NULL)
	{
		plCamDelete(self->camera);
		self->camera = NULL;
	}

	if(self->buffer != NULL)
	{
		SDL_FreeSurface(self->buffer);
		self->buffer = NULL;
	}

	if(self->window != NULL)
	{
		SDL_DestroyWindow(self->window);
		self->window = NULL;
		self->screen = NULL;
	}

	SDL_Quit();
}

