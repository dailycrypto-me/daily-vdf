#include "include/ProverWesolowski.h"
#include "include/VerifierWesolowski.h"
#include "include/types.h"
#include <iostream>
#include <string>
#include <random>
#include <fcntl.h>  /* For O_RDWR */
#include <unistd.h> /* For open() */

using namespace std;
using namespace vdf;

int main(int argc, char *argv[])
{
  if (argc != 4)
  { // command, lambda, t, lambdaRSW
    throw runtime_error("The parameters are: lambda, t, and lambdaRSW");
  }

  // read input parameters
  unsigned long lambda = stoul(argv[1]);
  unsigned long t = stoul(argv[2]);
  unsigned long lambdaRSW = stoul(argv[3]);

  // seed the C++ random engine properly
  char buf[50];
  int fd = open("/dev/urandom", O_RDONLY);
  if (fd < 0)
  {
    throw runtime_error("Could not open urandom.");
  }
  else
  {
    ssize_t result = read(fd, buf, sizeof buf);
    if (result < 0)
    {
      throw runtime_error("Could not read urandom.");
    }
    close(fd);
  }
  seed_seq sseed(buf, buf + 50);
  mt19937 engine(sseed);
  uniform_int_distribution<unsigned short> gen_byte(0, 255);

  // generate input x
  bytevec x(lambdaRSW / CHAR_BIT + 1);
  for (auto &xi : x)
  {
    xi = static_cast<decltype(x)::value_type>(gen_byte(engine));
  }

  VerifierWesolowski verifier(lambda, t, x, lambdaRSW);
  ProverWesolowski prover;
  auto solution = prover(verifier);
  if (verifier(solution))
  {
    cout << "Verified!" << endl
         << endl;
  }
  else
  {
    throw runtime_error("Not verified");
  }

  cout << "lambda:    " << lambda << endl
       << "t:         " << t << endl
       << "lambdaRSW: " << lambdaRSW << endl
       << endl;

  cout << "N:";
  for (const auto &ni : verifier.get_RSWPuzzle().get_N())
  {
    cout << ' ' << (int)ni;
  }
  cout << endl;

  cout << "x:";
  for (const auto &xi : verifier.get_RSWPuzzle().get_x())
  {
    cout << ' ' << (int)xi;
  }
  cout << endl
       << endl;

  cout << "Prover allocation: " << prover.durations[0].count() << endl
       << "Prover y:          " << prover.durations[1].count() << endl
       << "Prover proof:      " << prover.durations[2].count() << endl
       << "Prover hash:       " << prover.durations[3].count() << endl
       << endl;

  cout << "Verifier allocation: " << verifier.durations[0].count() << endl
       << "Verifier proof:      " << verifier.durations[2].count() << endl
       << "Verifier hash:       " << verifier.durations[3].count() << endl
       << endl;

  cout << "###################################################################" << endl
       << endl;

  return 0;
}
