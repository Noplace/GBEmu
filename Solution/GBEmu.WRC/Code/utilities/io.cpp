#include <stdio.h>
#include "io.h"

namespace core {
namespace io {

void ReadWholeFileBinary(const char* filename, uint8_t** data_pointer, size_t& data_length) {
  *data_pointer = nullptr;
  data_length = 0;
  FILE* fp = null;
  fopen_s(&fp,filename, "rb");
	if (fp==null)
		return;
  fseek(fp,0,SEEK_END);
//#ifdef _WIN64
  //data_length = _ftelli64(fp);
//#else
  data_length = ftell(fp);
//#endif
  fseek(fp,0,SEEK_SET);
  uint8_t* data = new uint8_t[data_length];
  fread(data,1,data_length,fp);
  fclose(fp);

  *data_pointer = data;
}

void DestroyFileBuffer(uint8_t** data_pointer) {
  SafeDeleteArray(data_pointer);
}

}
}