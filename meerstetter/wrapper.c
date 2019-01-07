#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include "ComPort/ComPort.h"
#include "MeComAPI/MePort.h"
#include "MeComAPI/MeCom.h"

char ERROR[100];

int fail_with_msg(char* error) {
  strcpy(ERROR, error);
  return 2;
}

int fail() {
  return 1;
}

char* get_last_error() {
  return ERROR;
}

int cmd_open(int port, int speed) {
  ComPort_Open(port, speed);
  return 0;
}

int cmd_close() {
  ComPort_Close();
  return 0;
}

int cmd_get_firmware_identity(int address, signed char* rv) {
  if (MeCom_GetIdentString(address, rv)) {
    return 0;
  }
  return fail();
}

int cmd_get_device_type(int address, int* buf) {
  MeParLongFields fields;
  if (MeCom_COM_DeviceType(address, &fields, MeGet)) {
    *buf = fields.Value;
    return 0;
  }
  return fail();
}

int cmd_set_target_temp(int address, float temp) {
  MeParFloatFields Fields;

  if (MeCom_TEC_Tem_TargetObjectTemp(address, 1, &Fields, MeGetLimits)) {
    if ((temp <= Fields.Max) && (temp >= Fields.Min)) {
      Fields.Value = temp;
      if (MeCom_TEC_Tem_TargetObjectTemp(address, 1, &Fields, MeSet)) {
        return 0;
      }
    } else {
      return fail_with_msg("target temperature out of limits");
    }
  }
  return fail();
}

int cmd_get_temp(int address, float* buf) {
  MeParFloatFields Fields;

  if (MeCom_TEC_Mon_ObjectTemperature(address, 1, &Fields, MeGet)) {
    *buf = Fields.Value;
    return 0;
  }

  return fail();
}


int cmd_get_int_param(int address, int code, int* buf) {
  MeParLongFields fields;

  if (MeCom_ParValuel(address, code, 1, &fields, MeGet)) {
    *buf = fields.Value;
    return 0;
  }
  return fail();
}

int cmd_get_float_param(int address, int code, float* buf) {
  MeParFloatFields fields;

  if (MeCom_ParValuef(address, code, 1, &fields, MeGet)) {
    *buf = fields.Value;
    return 0;
  }
  return fail();
}

int cmd_set_int_param(int address, int code, int value) {
  MeParLongFields fields;

  if (MeCom_ParValuel(address, code, 1, &fields, MeGetLimits)) {
    fields.Value = value;

    if (MeCom_ParValuel(address, code, 1, &fields, MeSet)) {
      return 0;
    }
    return fail();
  } else {
    return fail_with_msg("parameter value out of limits");
  }
}

int cmd_set_float_param(int address, int code, float value) {
  MeParFloatFields fields;

  if (MeCom_ParValuef(address, code, 1, &fields, MeGetLimits)) {
    fields.Value = value;

    if (MeCom_ParValuef(address, code, 1, &fields, MeSet)) {
      return 0;
    }
    return fail();
  } else {
    return fail_with_msg("parameter value out of limits");
  }
}

int cmd_reset_device(int address) {
  if (MeCom_ResetDevice(address)) {
    return 0;
  }
  return fail();
}