#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

using namespace std;

string usage = "Usage: stsCompare [-n <nb_finalAnalysisReports> <report1> <report2> ... | <report1> <report2>] [-p]";

int main(int argc, char* argv[]) {
  // Command line arguments parsing
  int nb_reports = 2, first_arg_report = 1;
  bool display_prop = false;
  if (argc < 3 || (argc > 3 && (string(argv[1]) != "-n" || 1 != sscanf(argv[2],"%d",&nb_reports) || argc < nb_reports + 3))) {
    cerr << usage << endl;
    return 1;
  }
  if (argc > nb_reports + 3)
    display_prop = true;
  if (argc > 3)
    first_arg_report = 3;
  ifstream* reports = new ifstream[nb_reports];
  for (int cur_arg = first_arg_report, i = 0; cur_arg < first_arg_report + nb_reports; cur_arg++, i++) {
    reports[i].open(argv[cur_arg]);
    if (!reports[i].is_open()) {
      cerr << usage << endl;
      return 1;
    }
  }

  // Droping the header (7 lines)
  char tmp;
  for (int i = 0; i < 7; i++) {
    for (int j = 0; j < nb_reports; j++) {
      while ((char) reports[j].get() != '\n');
    }
  }

  // Header for the results
  for (int j = 1; j <= nb_reports; j++)
    cout << "file" << j << "\t";
  cout << "test" << endl;

  // Extracting data for each statistical test
  int c[nb_reports][10];
  double pvalue[nb_reports],proportion[nb_reports];
  string* test = new string[nb_reports];
  bool ok = true;
  int cur = 0, nb_tests = 0;
  int nb_success[nb_reports], nb_total_success[nb_reports], nb_sequences[nb_reports], nb_total_sequences[nb_reports], diff_success[nb_reports];
  double diff_proportion[nb_reports];
  for (int j = 0; j < nb_reports; j++) {
    diff_success[j] = 0;
    nb_total_success[j] = 0;
    nb_total_sequences[j] = 0;
    diff_proportion[j] = 0.0;
  }
  while (ok) {
    // Retrieve next test data from the files
    for (int j = 0; j < nb_reports; j++) {
      // No more tests ?
      if ((char) reports[j].peek() == '\n') {
        ok = false;
        reports[j].get();
        break;
      }
      // Data parsing
      for (int i = 0; i < 10; i++)
        reports[j] >> c[j][i];
      string tmp;
      reports[j] >> pvalue[j] >> tmp;
      if (tmp == "*")
        reports[j] >> proportion[j];
      else
        sscanf(tmp.c_str(),"%lf",proportion + j);
      reports[j] >> test[j];
      if (test[j].size() == 1)
        reports[j] >> test[j];
      while ((char) reports[j].get() != '\n');
    }

    // End Loop when there is no more tests
    if (!ok) break;

    // Computing Data and printing
    for (int j = 0; j < nb_reports; j++) {
      if (test[j] != test[0]) {
        cout << '\t';
        ok = false;
        break;
      }
      nb_sequences[j] = 0;
      for (int i = 0; i < 10; i++)
        nb_sequences[j] += c[j][i];
      nb_total_sequences[j] += nb_sequences[j];
      nb_success[j] = proportion[j] * ((double) nb_sequences[j]);
      nb_total_success[j] += nb_success[j];
      diff_success[j] += nb_success[j] - nb_success[0];
      diff_proportion[j] += proportion[j] - proportion[0];
      cout << (display_prop ? proportion[j] : nb_success[j]);
      if (j > 0) {
        if (display_prop)
          cout << (proportion[j] >= proportion[0] ? "\t+" : "\t") << proportion[j] - proportion[0];
        else
          cout << (nb_success[j] >= nb_success[0] ? "\t+" : "\t") << nb_success[j] - nb_success[0];
      }
      cout << "\t";
    }
    cout << test[0] << endl;
    nb_tests++;
  }

  // More analysis
  cout << endl;
  for (int j = 0; j < nb_reports; j++)
    cout << (diff_success[j] < 0 ? "" : "+") << diff_success[j] << '\t';
  cout << "Sum diffs" << endl;
  for (int j = 0; j < nb_reports; j++)
    cout << (diff_proportion[j] < 0.0 ? "" : "+") << diff_proportion[j] << '\t';
  cout << "Sum diffs proportion\n" << endl;
  for (int j = 0; j < nb_reports; j++)
    cout << nb_total_success[j] << '/' << nb_total_sequences[j] << '\t';
  cout << "Sum success" << endl;
  for (int j = 0; j < nb_reports; j++)
    cout << ((double) nb_total_success[j]) / ((double) nb_total_sequences[j]) * 100.0 << '\t';
  cout << "Proportion" << endl;

  cout << "\nEnd." << endl;

  return 0;
}
