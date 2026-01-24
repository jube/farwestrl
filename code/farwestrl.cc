#include "bits/FarWestSystem.h"

#include "config.h"

int main()
{
  fw::FarWestSystem game(fw::FarWestDataDirectory);
  return game.run();
}
