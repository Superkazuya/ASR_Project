#include "config.h"
#include "mfcc.h"
#include "sample_proc.h"
#include "record.h"

static void event_handler();
void exit_handler();


static void event_handler()
{
  static void (*handler[OP_LAST])() = {
    [OP_EXIT] = exit_handler
  };
  (*handler[status])();
}

void exit_handler()
{
}

int main()
{
  hamming_init();
  mfcc_init(32000);
  record_init(32000);
  //record start
  while(1);

  event_handler();
}
