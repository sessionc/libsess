/**
 * \file
 * Example parser invoker.
 *
 * \headerfile "ScribbleProtocolLexer.h"
 * \headerfile "ScribbleProtocolParser.h"
 * \hedaerfile "parser.h"
 */

#include <stdio.h>
#include <stdlib.h>

#include "ScribbleProtocolLexer.h"
#include "ScribbleProtocolParser.h"

#include "parser.h"

/**
 * \brief Parser invoker. 
 *
 * Invokes parser followed by noact.
 */
int ANTLR3_CDECL main(int argc, char *argv[])
{

  printf("%s: Sample parser invoker.\n", argv[0]);
  if (argc < 2) {
    fprintf(stderr, "\nUsage: %s scribble_filename\n\n", argv[0]);
    return -1;
  }

  parse(argv[1]);

  printf("%s: Parser invoked successfully.\n", argv[0]);
  return EXIT_SUCCESS;
}
