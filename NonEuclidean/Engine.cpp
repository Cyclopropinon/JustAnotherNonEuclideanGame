#include "Engine.h"
#include "Physical.h"
#include "Levels.h"
#if defined(_WIN32)
  #include <GL/wglew.h>
#else
  #include <GL/glew.h>
  #include <GL/freeglut.h>
#endif
#include <cmath>
#include <iostream>
#include <algorithm>
#include <string>

Engine* GH_ENGINE = nullptr;
Player* GH_PLAYER = nullptr;
const Input* GH_INPUT = nullptr;
int GH_REC_LEVEL = 0;
int64_t GH_FRAME = 0;

Engine::Engine() {
  // GLUT initialisieren (falls noch nicht geschehen)
  int argc = 1;
  char* argv[1] = {(char*)"Spiel"};
  glutInit(&argc, argv);  GH_ENGINE = this;
  GH_INPUT = &input;
  isFullscreen = false;

  isGood = InitOSWrapper();

  CreateGLWindow();
  InitGLObjects();
  SetupInputs();

  player.reset(new Player);
  GH_PLAYER = player.get();

  vScenes.push_back(std::shared_ptr<Scene>(new Level1));
  vScenes.push_back(std::shared_ptr<Scene>(new Level2(3)));
  vScenes.push_back(std::shared_ptr<Scene>(new Level2(6)));
  vScenes.push_back(std::shared_ptr<Scene>(new Level3));
  vScenes.push_back(std::shared_ptr<Scene>(new Level4));
  vScenes.push_back(std::shared_ptr<Scene>(new Level5));
  vScenes.push_back(std::shared_ptr<Scene>(new Level6));
  vScenes.push_back(std::shared_ptr<Scene>(new Level7));

  LoadScene(0);

  sky.reset(new Sky);
}

Engine::~Engine() {
  DestroyGLWindow();
}

int Engine::Run() {
  EnterMessageLoop();
  DestroyGLObjects();
  return 0;
}

void Engine::RenderString(float x, float y, void *font, const unsigned char* string, float r, float g, float b)
{  
  // OpenGL auf 2D-Rendering umstellen
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, iWidth, 0, iHeight);  // 2D-Projektion passend zur Fenstergröße

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glDisable(GL_DEPTH_TEST);

  glColor3f(r, g, b); 
  glRasterPos2f(x, y);

  glutBitmapString(font, string);
  
  glEnable(GL_DEPTH_TEST);

  // Zurück zu 3D-Rendering
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

inline void Engine::RenderString(float x, float y, void *font, std::string string, float r, float g, float b)
{
  RenderString(x, y, font, reinterpret_cast<const unsigned char*>(string.c_str()), r, g, b);
}

void Engine::DrawPlayerPosition() {
  if (!GH_PLAYER) return;

  // Spielerposition als Text formatieren
  std::string posText = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA_DIGGA_WARUM_FUNKTIONIERT_DIESE_SCHEISSE_NICHT??????";
  /*"Pos: (" + 
                        std::to_string(GH_PLAYER->pos.x) + ", " + 
                        std::to_string(GH_PLAYER->pos.y) + ", " + 
                        std::to_string(GH_PLAYER->pos.z) + ")";*/

  // Position unten links
  int textX = 10;  // Linker Rand
  int textY = 10;  // Unterer Rand

  // Text Zeichen für Zeichen zeichnen
  RenderString(textX, textY, GLUT_BITMAP_9_BY_15, posText, 0, 0, 0);
}

void Engine::PeriodicRender(int64_t &cur_ticks) {

  //Used fixed time steps for updates
  const int64_t new_ticks = timer.GetTicks();
  for (int i = 0; cur_ticks < new_ticks && i < GH_MAX_STEPS; ++i) {
    Update();
    cur_ticks += ticks_per_step;
    GH_FRAME += 1;
    input.EndFrame();
  }
  cur_ticks = (cur_ticks < new_ticks ? new_ticks: cur_ticks);

  //Setup camera for rendering
  const float n = GH_CLAMP(NearestPortalDist() * 0.5f, GH_NEAR_MIN, GH_NEAR_MAX);
  main_cam.worldView = player->WorldToCam();
  main_cam.SetSize(iWidth, iHeight, n, GH_FAR);
  main_cam.UseViewport();

  // Get camera position
  std::cout << "Camera Position: " << GH_PLAYER->pos.x << ",  " << GH_PLAYER->pos.y << ",  " << GH_PLAYER->pos.z << ",  " << iWidth  << ",  " << iHeight << ",  " << -1 << " FPS    \r";

  //Render scene
  GH_REC_LEVEL = GH_MAX_RECURSION;
  Render(main_cam, 0, nullptr);

  // Zeichne Spielerposition als Overlay-Text
  DrawPlayerPosition();
}

void Engine::LoadScene(int ix) {
  //Clear out old scene
  if (curScene) { curScene->Unload(); }
  vObjects.clear();
  vPortals.clear();
  player->Reset();

  //Create new scene
  curScene = vScenes[ix];
  curScene->Load(vObjects, vPortals, *player);
  vObjects.push_back(player);
}

void Engine::Update() {
  //Update
  for (size_t i = 0; i < vObjects.size(); ++i) {
    assert(vObjects[i].get());
    vObjects[i]->Update();
  }

  //Collisions
  //For each physics object
  for (size_t i = 0; i < vObjects.size(); ++i) {
    Physical* physical = vObjects[i]->AsPhysical();
    if (!physical) { continue; }
    Matrix4 worldToLocal = physical->WorldToLocal();

    //For each object to collide with
    for (size_t j = 0; j < vObjects.size(); ++j) {
      if (i == j) { continue; }
      Object& obj = *vObjects[j];
      if (!obj.mesh) { continue; }

      //For each hit sphere
      for (size_t s = 0; s < physical->hitSpheres.size(); ++s) {
        //Brings point from collider's local coordinates to hits's local coordinates.
        const Sphere& sphere = physical->hitSpheres[s];
        Matrix4 worldToUnit = sphere.LocalToUnit() * worldToLocal;
        Matrix4 localToUnit = worldToUnit * obj.LocalToWorld();
        Matrix4 unitToWorld = worldToUnit.Inverse();

        //For each collider
        for (size_t c = 0; c < obj.mesh->colliders.size(); ++c) {
          Vector3 push;
          const Collider& collider = obj.mesh->colliders[c];
          if (collider.Collide(localToUnit, push)) {
            //If push is too small, just ignore
            push = unitToWorld.MulDirection(push);
            vObjects[j]->OnHit(*physical, push);
            physical->OnCollide(*vObjects[j], push);

            worldToLocal = physical->WorldToLocal();
            worldToUnit = sphere.LocalToUnit() * worldToLocal;
            localToUnit = worldToUnit * obj.LocalToWorld();
            unitToWorld = worldToUnit.Inverse();
          }
        }
      }
    }
  }

  //Portals
  for (size_t i = 0; i < vObjects.size(); ++i) {
    Physical* physical = vObjects[i]->AsPhysical();
    if (physical) {
      for (size_t j = 0; j < vPortals.size(); ++j) {
        if (physical->TryPortal(*vPortals[j])) {
          break;
        }
      }
    }
  }
}

void Engine::Render(const Camera& cam, GLuint curFBO, const Portal* skipPortal) {
  //Clear buffers
  if (GH_USE_SKY) {
    glClear(GL_DEPTH_BUFFER_BIT);
    sky->Draw(cam);
  } else {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  //Create queries (if applicable)
  GLuint queries[GH_MAX_PORTALS];
  GLuint drawTest[GH_MAX_PORTALS];
  assert(vPortals.size() <= GH_MAX_PORTALS);
  if (occlusionCullingSupported) {
    glGenQueriesARB((GLsizei)vPortals.size(), queries);
  }

  //Draw scene
  for (size_t i = 0; i < vObjects.size(); ++i) {
    vObjects[i]->Draw(cam, curFBO);
  }

  //Draw portals if possible
  if (GH_REC_LEVEL > 0) {
    //Draw portals
    GH_REC_LEVEL -= 1;
    if (occlusionCullingSupported && GH_REC_LEVEL > 0) {
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      glDepthMask(GL_FALSE);
      for (size_t i = 0; i < vPortals.size(); ++i) {
        if (vPortals[i].get() != skipPortal) {
          glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queries[i]);
          vPortals[i]->DrawPink(cam);
          glEndQueryARB(GL_SAMPLES_PASSED_ARB);
        }
      }
      for (size_t i = 0; i < vPortals.size(); ++i) {
        if (vPortals[i].get() != skipPortal) {
          glGetQueryObjectuivARB(queries[i], GL_QUERY_RESULT_ARB, &drawTest[i]);
        }
      };
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glDepthMask(GL_TRUE);
      glDeleteQueriesARB((GLsizei)vPortals.size(), queries);
    }
    for (size_t i = 0; i < vPortals.size(); ++i) {
      if (vPortals[i].get() != skipPortal) {
        if (occlusionCullingSupported && (GH_REC_LEVEL > 0) && (drawTest[i] == 0)) {
          continue;
        } else {
          vPortals[i]->Draw(cam, curFBO);
        }
      }
    }
    GH_REC_LEVEL += 1;
  }
  
#if 0
  //Debug draw colliders
  for (size_t i = 0; i < vObjects.size(); ++i) {
    vObjects[i]->DebugDraw(cam);
  }
#endif
}

void Engine::InitGLObjects() {
  //Initialize extensions
  glewExperimental = GL_TRUE; 
  glewInit();

  //Basic global variables
  glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);

  //Check GL functionality
  glGetQueryiv(GL_SAMPLES_PASSED_ARB, GL_QUERY_COUNTER_BITS_ARB, &occlusionCullingSupported);

  EnableVSync();
}

void Engine::DestroyGLObjects() {
  curScene->Unload();
  vObjects.clear();
  vPortals.clear();
}

float Engine::NearestPortalDist() const {
  float dist = FLT_MAX;
  for (size_t i = 0; i < vPortals.size(); ++i) {
    dist = GH_MIN(dist, vPortals[i]->DistTo(player->pos));
  }
  return dist;
}
