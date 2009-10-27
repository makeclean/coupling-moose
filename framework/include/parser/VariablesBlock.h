#ifndef VARIABLESBLOCK_H
#define VARIABLESBLOCK_H

#include "ParserBlock.h"

class VariablesBlock: public ParserBlock
{
public:
  VariablesBlock(const std::string & reg_id, const std::string & real_id, ParserBlock * parent, Parser & parser_handle);

  virtual void execute();
  virtual void copyNodalValues(const std::string &system_name);
};

  

#endif //VARIABLESBLOCK_H
