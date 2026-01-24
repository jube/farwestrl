#include <gf2/core/Log.h>
#include <gf2/core/Random.h>

#include "bits/Names.h"

int main()
{
  gf::Log::debug("male: {}, female: {}, surname: {}", fw::compute_max_length(fw::NameType::MaleName), fw::compute_max_length(fw::NameType::FemaleName), fw::compute_max_length(fw::NameType::Surname));

  gf::Random random;

  gf::Log::info("Female name: {}", fw::generate_random_white_female_name(&random));
  gf::Log::info("Male name: {}", fw::generate_random_white_male_name(&random));
}
