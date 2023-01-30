#include <cstdlib>

#include <limits>
#include <iostream>
#include <fstream>
#include <tuple>
#include <algorithm>
#include <string>
#include <TFile.h>
#include <TGraph.h>
#include <TH1.h>
#include <TGraphErrors.h>
#include <TString.h>
#include <TKey.h>

#include "Trend.h"

using namespace std;

[[ maybe_unused ]]
static const char * bold = "\e[1m", * normal = "\e[0m";

TGraph * ReadTxt (const fs::path& file, int lastRun, std::list<int> bad_runs, const char * title = "")
{
    assert(fs::exists(file));

    vector<float> runs, values, errors;
    runs.reserve(2000);
    values.reserve(2000);
    errors.reserve(2000);

    ifstream f;
    f.open(file.c_str());
    while (f.good()) {
        float run, value, error;
        f >> run >> value >> error;
        if (f.eof() || run >= lastRun) break;
        runs.push_back(run);
        values.push_back(value);
        errors.push_back(error);
    }
    f.close();

    int N = runs.size();
    runs.push_back(lastRun);
    //auto name = file.stem().c_str();
    //auto h = new TH1D(name, title, N, runs.data());

    auto g = new TGraphErrors(N);
    g->SetTitle(title);
    for (int i = 0; i < N; ++i) {
        //h->SetBinContent(i, values.at(i));
        //h->SetBinError  (i, errors.at(i));
        if(std::find(bad_runs.begin(),bad_runs.end(),runs.at(i))!=bad_runs.end()){
	  continue;};
        if(values.at(i)>50. or values.at(i)<-30.){
	  cout << "value is " << values.at(i) << endl;};
        g->SetPoint(i, runs.at(i), values.at(i));
        g->SetPointError(i, 0., errors.at(i));
    }
    //return h;
    return g;
}

std::list<int> Bad_runs (const fs::path& file1, const fs::path& file2, int lastRun)
{
    assert(fs::exists(file1));
    assert(fs::exists(file2));

    vector<float> runs1, values1, errors1;
    runs1.reserve(2000);
    values1.reserve(2000);
    errors1.reserve(2000);
    vector<float> runs2, values2, errors2;
    runs2.reserve(2000);
    values2.reserve(2000);
    errors2.reserve(2000);
    std::list<int> bad_runs;

    ifstream f1;
    f1.open(file1.c_str());
    while (f1.good()) {
        float run1, value1, error1;
        f1 >> run1 >> value1 >> error1;
        if (f1.eof() || run1 >= lastRun) break;
        runs1.push_back(run1);
        values1.push_back(value1);
        errors1.push_back(error1);
    }
    f1.close();

    ifstream f2;
    f2.open(file2.c_str());
    while (f2.good()) {
        float run2, value2, error2;
        f2 >> run2 >> value2 >> error2;
        if (f2.eof() || run2 >= lastRun) break;
        runs2.push_back(run2);
        values2.push_back(value2);
        errors2.push_back(error2);
    }
    f2.close();

    int N = runs1.size();
    runs1.push_back(lastRun);
    runs2.push_back(lastRun);

    for (int i = 0; i < N; ++i) {
        if(errors1.at(i)>5.){
	  bad_runs.push_back(runs1.at(i));
	};
	if(errors2.at(i)>5.){
	  bad_runs.push_back(runs2.at(i));
	};
    }
    return bad_runs;
}


int plot (int layer,const std::string var, const std::string dir, const std::string folder)
{
    Trend::CMS = "#scale[1.5]{#bf{CMS}} #it{Preliminary}"; // TODO: right before approval
    //Trend::CMS = "#scale[1.5]{#bf{CMS}}"; // TODO: right before CWR
    // otherwise the tool uses by default "CMS Internal"

    //int firstRun = atoi(argv[1]), lastRun = atoi(argv[2]);
    int firstRun = 355100, lastRun = 362760;
    std::string Generic_textfile = (boost::format("inputs/%1%/%2%_%3%_L%4%_Generic.txt") % folder % var % dir % layer).str(); 
    std::string Template_textfile = (boost::format("inputs/%1%/%2%_%3%_L%4%_Template.txt") % folder % var % dir % layer).str(); 
    std::string jsonfile = (boost::format("inputs/linesBPix_Layer%1%.json") % layer).str();
    std::string outputfile =(boost::format("plots/%1%/BPix_%2%_%3%_Layer%4%") % folder % var % dir % layer).str();
    //std::string outputfile =(boost::format("BPix_%1%_%2%_Layer%3%") % var % dir % layer).str();
    char * outputfile_char = new char [outputfile.length()+1];
    std::strcpy(outputfile_char,outputfile.c_str());

    const Run2Lumi GetLumi("deliveredLumiRun3.txt", firstRun, lastRun);

    fs::path json = jsonfile;
    assert(fs::exists(json));
    pt::ptree lines;
    pt::read_json(json.c_str(), lines);

    std::list<int> bad_runs = Bad_runs(Generic_textfile, Template_textfile, lastRun);
    auto hGen = ReadTxt(Generic_textfile, lastRun, bad_runs, "Generic reconstruction");
    hGen->SetLineColor(kWhite);
    hGen->SetMarkerColor(kRed);
    hGen->SetMarkerStyle(kFullCircle);
    hGen->SetMarkerSize(0.6);
    hGen->SetFillColorAlpha(kRed, 0.2);

    auto hTemp = ReadTxt(Template_textfile, lastRun, bad_runs, "Template reconstruction");
    hTemp->SetLineColor(kWhite);
    hTemp->SetMarkerColor(kBlue);
    hTemp->SetMarkerStyle(kFullSquare);
    hTemp->SetMarkerSize(0.6);
    hTemp->SetFillColorAlpha(kBlue, 0.2);
    
    //double max_hGen = hGen->GetMaximum();
    //double max_hTemp = hTemp->GetMaximum();
    //int y_low;
    //int y_high;
    //if(max_hGen>max_hTemp){
    //  y_low = -abs(max_hGen);
    //  y_high = abs(max_hGen);}
    //else{
    //  y_low = -abs(max_hTemp);
    //  y_high = abs(max_hTemp);};
    //int y_low = -30;
    //int y_high = 80;
    int y_low;
    int y_high;
    std::string direction = "undefined";
    std::string label = "undefined";
    if(layer==1 && var=="mu" && dir=="x"){
      label = "#mu [#mum]";
      direction = "r-#phi";
      y_low = -5;
      y_high = 50;
    }
    if(layer==2 && var=="mu" && dir=="x"){
      label = "#mu [#mum]";
      direction = "r-#phi";
      y_low = -25;
      y_high = 20;
    }
    if(layer==3 && var=="mu" && dir=="x"){
      label = "#mu [#mum]";
      direction = "r-#phi";
      y_low = -5;
      y_high = 10;
    }
    if(layer==4 && var=="mu" && dir=="x"){
      label = "#mu [#mum]";
      direction = "r-#phi";
      y_low = -10;
      y_high = 15;
    }
    if(layer==1 && var=="sigma" && dir=="x"){
      label = "#sigma [#mum]";
      direction = "r-#phi";
      y_low = 10;
      y_high = 50;
    }
    if(layer==2 && var=="sigma" && dir=="x"){
      label = "#sigma [#mum]";
      direction = "r-#phi";
      y_low = 5;
      y_high = 25;
    }
    if(layer==3 && var=="sigma" && dir=="x"){
      label = "#sigma [#mum]";
      direction = "r-#phi";
      y_low = 5;
      y_high = 25;
    }
    if(layer==4 && var=="sigma" && dir=="x"){
      label = "#sigma [#mum]";
      direction = "r-#phi";
      y_low = 17;
      y_high = 50;
    }
    if(layer==1 && var=="mu" && dir=="z"){
      label = "#mu [#mum]";
      direction = "z";
      y_low = -10;
      y_high = 15;
    }
    if(layer==2 && var=="mu" && dir=="z"){
      label = "#mu [#mum]";
      direction = "z";
      y_low = -5;
      y_high = 10;
    }
    if(layer==3 && var=="mu" && dir=="z"){
      label = "#mu [#mum]";
      direction = "z";
      y_low = -5;
      y_high = 10;
    }
    if(layer==4 && var=="mu" && dir=="z"){
      label = "#mu [#mum]";
      direction = "z";
      y_low = -10;
      y_high = 15;
    }
    if(layer==1 && var=="sigma" && dir=="z"){
      label = "#sigma [#mum]";
      direction = "z";
      y_low = 40;
      y_high = 120;
    }
    if(layer==2 && var=="sigma" && dir=="z"){
      label = "#sigma [#mum]";
      direction = "z";
      y_low = 20;
      y_high = 60;
    }
    if(layer==3 && var=="sigma" && dir=="z"){
      label = "#sigma [#mum]";
      direction = "z";
      y_low = 15;
      y_high = 40;
    }
    if(layer==4 && var=="sigma" && dir=="z"){
      label = "#sigma [#mum]";
      direction = "z";
      y_low = 40;
      y_high = 80;
    }

    auto legend_label = boost::format("BPIX L%1%, %2%-direction (p_{T} > 12 GeV)") %layer % direction; 
    string title = legend_label.str();
    {
        Trend t(outputfile_char, title.c_str(), label.c_str(), y_low, y_high, lines, GetLumi);
        t.lgd.SetHeader(title.c_str());
        t.lgd.SetTextSize(0.04);
        t(hTemp, "P2", "pf");
        t(hGen , "P2", "pf");
    }

    return EXIT_SUCCESS;
}






int main ()
{
    Trend::CMS = "#scale[1.5]{#bf{CMS}} #it{Preliminary}"; // TODO: right before approval
    //Trend::CMS = "#scale[1.5]{#bf{CMS}}"; // TODO: right before CWR
    // otherwise the tool uses by default "CMS Internal"
    //TFile* f = TFile::Open("BPIX.root","NEW");
    std::string variables[2] = {"mu","sigma"};
    std::string directions[2] = {"x","z"};
    for (int layer = 1; layer < 5; layer++){
      for(int i=0; i < 2; i++){
	for(int j=0; j < 2; j++){
	  std::cout << "layer: " << layer << " var: " << variables[i] << "direction:" << directions[j] <<"\n";
	  plot(layer,variables[i],directions[j],"Muon_merged");
        };
      };
    };
    //f->Close();
    return EXIT_SUCCESS;
}
