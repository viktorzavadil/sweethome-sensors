#ifndef _SWEETHOME_TMEP
#define _SWEETHOME_TMEP

namespace tmep {

  struct TmepOption {
    char *host;
    int port = 80;
    char *sensorGuid;
  };

  void sendValue(TmepOption *option, String value);

}

#endif
