#ifndef TYPES_H_
#define TYPES_H_

struct ControlData {
  uint16_t UniqueId;
  uint16_t Throttle;
  uint16_t Yaw;
  uint16_t Pitch;
  uint16_t Roll;
  uint16_t Aux1;
  uint16_t Aux2;
  uint16_t Aux3;
  uint16_t Aux4;
  uint16_t Aux5;
  uint16_t Aux6;
  uint16_t Swd1;
  uint16_t Swd2;
};

struct RxConfig {
  uint8_t Id;
  uint8_t RadioChannel;
  int8_t  ThrottleTrim;
  int8_t  YawTrim;
  int8_t  PitchTrim;
  int8_t  RollTrim;
  char    Name[10];
};

typedef struct menu_node_item {
  uint8_t Id;
  char* Menu;
  struct menu_node* Item;
  //void (*LookupFunc)(bool*);
} MenuNodeItem;

typedef struct menu_node {
  char* Title;
  menu_node* Prev;
  MenuNodeItem* Items;
  uint8_t Index;
  uint8_t ItemCount;
  uint8_t LookupItemCount;
  uint8_t ParentId;
  char* ParentMenu;
} MenuNode;

#endif /* TYPES_H_ */
