#ifndef TYPES_H_
#define TYPES_H_

struct ControlData {
  uint32_t Token;
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
  uint8_t   Id;
  uint8_t   RadioChannel;
  uint32_t  Token;
  int8_t    ThrottleTrim;
  int8_t    YawTrim;
  int8_t    PitchTrim;
  int8_t    RollTrim;
  char      Name[11];
};

struct TxBindData {
  char TxIdentifier[10];
  uint32_t Token;
};

struct RxBindData {
  char RxIdentifier[10];
  uint32_t Token;
};

struct RateData {
  uint16_t LoopRate;
  uint16_t ButtonCheckRate;
  uint16_t DataGetRate;
  uint16_t RadioRate;
  uint16_t CPPMRate;
};

typedef struct menu_node_item {
  uint8_t Id;
  char* Menu;
  struct menu_node* Item;
  bool Selectable;
} MenuNodeItem;

typedef struct menu_node {
  char* Title;
  menu_node* Prev;
  MenuNodeItem* Items;
  uint8_t Index;
  uint8_t ScrollIndex;
  uint8_t ItemCount;
  uint8_t ParentId;
  char* ParentMenu;
  void (*ExecFunc)(bool);
} MenuNode;

typedef struct menu_item_edit {
  char* Value;
  uint8_t Index;
  uint8_t MaxLength;
} MenuItemEdit;

typedef struct menu_pagination  {
  uint8_t StartIndex;
  uint8_t EndIndex;
} MenuPagination;

typedef enum {
  buttonPress,
  lowVoltage,
  throttleWarning,
  radioBinding
} BuzzerBeepPattern;

#endif /* TYPES_H_ */
