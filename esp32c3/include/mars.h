// Mars time (Allen & Mahoney) and the sol / weem / moom calendar.
// Same math as index.html; no Arduino dependencies.
#pragma once
#include <math.h>

const double TAI_MINUS_UTC  = 37.0;          // update if a leap second is added
const double SOL_IN_DAYS    = 1.0274912517;
const double CYCLE_SOLS     = 3 * 668.5991;  // 3 Mars years

const char *const MOOMS[20] = {
  "Arcadia", "Borealis", "Cydonia", "Daedalia", "Elysium", "Fortuna", "Gale", "Hellas", "Isidis", "Jezero",
  "Kasei", "Lyot", "Meridiani", "Noachis", "Olympus", "Pavonis", "Quenisset", "Reull", "Syrtis", "Tharsis"};
const char *const SOLS[10] = {
  "Sunsol", "Mercurisol", "Venusol", "Terrasol", "Phobosol",
  "Deimosol", "Jovisol", "Saturnisol", "Uranisol", "Neptunisol"};

struct MarsTime {
  double msd;          // Mars Sol Date
  int h, m, s;         // Coordinated Mars Time
  long cycle;          // 3-Mars-year cycle, from 1
  bool mini;           // in the closing mini weem
  int day;             // day of moom 1..100, or of mini weem
  int moom;            // 1..20 (0 in mini weem)
  int weem;            // 1..10 (0 in mini weem)
  const char *solName;
  const char *moomName;
};

inline MarsTime marsTime(double unixSeconds) {
  MarsTime t;
  double jdTT = unixSeconds / 86400.0 + 2440587.5 + (TAI_MINUS_UTC + 32.184) / 86400.0;
  t.msd = (jdTT - 2405522.0028779) / SOL_IN_DAYS;

  long sol = (long)floor(t.msd);
  long secs = (long)((t.msd - sol) * 86400.0);
  t.h = secs / 3600;
  t.m = secs % 3600 / 60;
  t.s = secs % 60;

  auto start = [](long n) { return (long)floor(n * CYCLE_SOLS); };
  long n = (long)floor(sol / CYCLE_SOLS);
  while (start(n) > sol) n--;
  while (start(n + 1) <= sol) n++;
  long d = sol - start(n);

  t.cycle = n + 1;
  t.mini = d >= 2000;
  t.day  = t.mini ? d - 1999 : d % 100 + 1;
  t.moom = t.mini ? 0 : d / 100 + 1;
  t.weem = t.mini ? 0 : d % 100 / 10 + 1;
  t.solName  = SOLS[(t.day - 1) % 10];
  t.moomName = t.mini ? "Utopia" : MOOMS[t.moom - 1];
  return t;
}
