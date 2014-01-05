#include "Brainterface.h"
#include <vector>
#include <cstdlib>
#include <ctime>

/*		BRALAB		*/

int main(int argc, char *argv[]){
	srand(time(0));
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "Bra.Lab");
	Maindow md(3);
	if (argc>1){
		for (int a=1;a<argc;++a){
			if (argv[a][0]=='v'){
				md.verbose=true;
			}
		}
	}	
	md.brawing.graph.push_back(0);
	md.brawing.red.push_back(0);
	md.brawing.green.push_back(0);
	
	return app->run(md);
}
