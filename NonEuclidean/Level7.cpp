#include "Level7.h"
#include "Tunnel.h"
#include "Ground.h"

void Level7::Load(PObjectVec& objs, PPortalVec& portals, Player& player) {
  constexpr int abstandX = 5;
  constexpr int abstandZ = -5;
  constexpr float rotation = 1;

  std::shared_ptr<Tunnel> tunnel1(new Tunnel(Tunnel::SLOPE));
  tunnel1->pos = Vector3(0, 0, 0);
  tunnel1->scale = Vector3(1, 1, 5);
  tunnel1->euler.y = rotation * GH_PI;
  objs.push_back(tunnel1);

  std::shared_ptr<Ground> ground1(new Ground(true));
  ground1->scale *= Vector3(1, 40, 1);
  objs.push_back(ground1);

  std::shared_ptr<Tunnel> tunnel2(new Tunnel(Tunnel::SLOPE));
  tunnel2->pos = Vector3(200, 0, 0);
  tunnel2->scale = Vector3(1, 1, 5);
  objs.push_back(tunnel2);

  std::shared_ptr<Ground> ground2(new Ground(true));
  ground2->pos = Vector3(200, 0, 0);
  ground2->scale *= Vector3(1, 2, 1);
  ground2->euler.y = GH_PI;
  objs.push_back(ground2);

  std::shared_ptr<Portal> portal1(new Portal());
  tunnel1->SetDoor1(*portal1);
  portals.push_back(portal1);

  std::shared_ptr<Portal> portal2(new Portal());
  tunnel1->SetDoor2(*portal2);
  portals.push_back(portal2);

  std::shared_ptr<Portal> portal3(new Portal());
  tunnel2->SetDoor1(*portal3);
  portal3->euler.y -= GH_PI;
  portals.push_back(portal3);

  std::shared_ptr<Portal> portal4(new Portal());
  tunnel2->SetDoor2(*portal4);
  portal4->euler.y -= GH_PI;
  portals.push_back(portal4);

  Portal::Connect(portal1, portal4);
  Portal::Connect(portal2, portal3);

  std::shared_ptr<Tunnel> tunnel3(new Tunnel(Tunnel::NORMAL));
  tunnel3->pos = Vector3(abstandX - 2.4f, 0, abstandZ - 1.8f);
  tunnel3->scale = Vector3(1, 1, 4.8f);
  objs.push_back(tunnel3);

  std::shared_ptr<Tunnel> tunnel4(new Tunnel(Tunnel::NORMAL));
  tunnel4->pos = Vector3(abstandX + 2.4f, 0, abstandZ + 0);
  tunnel4->scale = Vector3(1, 1, 0.2f);
  objs.push_back(tunnel4);

  /*std::shared_ptr<Ground> ground(new Ground());
  ground->scale *= 1.2f;
  objs.push_back(ground);*/

  std::shared_ptr<Portal> portal5(new Portal());
  tunnel3->SetDoor1(*portal5);
  portals.push_back(portal5);

  std::shared_ptr<Portal> portal6(new Portal());
  tunnel4->SetDoor1(*portal6);
  portals.push_back(portal6);

  std::shared_ptr<Portal> portal7(new Portal());
  tunnel3->SetDoor2(*portal7);
  portals.push_back(portal7);

  std::shared_ptr<Portal> portal8(new Portal());
  tunnel4->SetDoor2(*portal8);
  portals.push_back(portal8);

  Portal::Connect(portal5, portal6);
  Portal::Connect(portal7, portal8);

  player.SetPosition(Vector3(0, GH_PLAYER_HEIGHT - 2, 8));
}
