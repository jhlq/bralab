#ifndef BRAINTERFACE_H
#define BRAINTERFACE_H

#include <gtkmm.h>
#include "Brain.h"

class Madialog : public Gtk::Dialog{
public:
	Madialog(int nn,Brain* br);
	Gtk::Frame axonframe,threshframe,leakframe,habframe;
	std::vector<Glib::RefPtr<Gtk::Adjustment> > axonadj,threshadj,leakadj,habadj;
	std::vector<Gtk::SpinButton*> axonspin,threshspin,leakspin,habspin;
	Gtk::ScrolledWindow axonscroll,threshscroll,leakscroll,habscroll;
	Gtk::Grid axongrid,threshgrid,leakgrid,habgrid;
	Gtk::ButtonBox axonbox;
	Gtk::Box hb;
	
	void on_maxon(int);
	void on_mthresh(int);
	void on_mleak(int);
	void on_mhab(int);
	
	Brain* brain;
};
Madialog::Madialog(int nn,Brain* br):		axonframe("Axons"),
											threshframe("Thresholds"),
											leakframe("Leaks"),
											habframe("Habituation"),
											hb(Gtk::ORIENTATION_HORIZONTAL)
								{
	brain=br;
	set_default_size(600, 300);
	get_content_area()->pack_start(hb);
	hb.pack_start(axonframe);
	axonframe.add(axonscroll);
	axonscroll.add(axongrid);
	hb.pack_start(threshframe);
	threshframe.add(threshscroll);
	threshscroll.add(threshgrid);
	hb.pack_start(leakframe,Gtk::PACK_EXPAND_WIDGET);
	leakframe.add(leakscroll);
	leakscroll.add(leakgrid);
	hb.pack_start(habframe);
	habframe.add(habscroll);
	habscroll.add(habgrid);
	
	int in=-1;
	for (auto nit=brain->get_nl().begin();nit!=brain->get_nl().end();++nit){

		++in;
		axonadj.push_back(Gtk::Adjustment::create(brain->get_mvaxons()[in], 0, 1000.000, 0.1, 100.0, 0.0));
		threshadj.push_back(Gtk::Adjustment::create(brain->get_mvthresh()[in], 0, 1000.000, 0.1, 100.0, 0.0));
		leakadj.push_back(Gtk::Adjustment::create(brain->get_mvleak()[in], 0, 1000.000, 0.1, 100.0, 0.0));
		habadj.push_back(Gtk::Adjustment::create(brain->get_mvhab()[in], 0, 1000.000, 0.1, 100.0, 0.0));
	}
	
	in=-1;
	for (auto nit=brain->get_nl().begin();nit!=brain->get_nl().end();++nit){
		++in;
		axonspin.push_back(new Gtk::SpinButton(axonadj[in],0.3,3));
		threshspin.push_back(new Gtk::SpinButton(threshadj[in],0.3,3));
		leakspin.push_back(new Gtk::SpinButton(leakadj[in],0.3,3));
		habspin.push_back(new Gtk::SpinButton(habadj[in],0.3,3));
		axonadj[in]->signal_value_changed().connect( sigc::bind<int>(sigc::mem_fun(*this, &Madialog::on_maxon),in) );
		threshadj[in]->signal_value_changed().connect( sigc::bind<int>(sigc::mem_fun(*this, &Madialog::on_mthresh),in) );
		leakadj[in]->signal_value_changed().connect( sigc::bind<int>(sigc::mem_fun(*this, &Madialog::on_mleak),in) );
		habadj[in]->signal_value_changed().connect( sigc::bind<int>(sigc::mem_fun(*this, &Madialog::on_mhab),in) );
		axongrid.attach(*axonspin[in], 1, in, 1, 1);
		threshgrid.attach(*threshspin[in], 1, in, 1, 1);
		leakgrid.attach(*leakspin[in], 1, in, 1, 1);		
		habgrid.attach(*habspin[in], 1, in, 1, 1);		
	}	
	show_all_children();
}
void Madialog::on_maxon(int n){
	brain->set_mvaxons(n,axonspin[n]->get_value());
}
void Madialog::on_mthresh(int n){
	brain->set_mvthresh(n,threshspin[n]->get_value());
}
void Madialog::on_mleak(int n){
	brain->set_mvleak(n,leakspin[n]->get_value());
}
void Madialog::on_mhab(int n){
	brain->set_mvhab(n,habspin[n]->get_value());
}

class Marea : public Gtk::DrawingArea{
public:
	Marea(){}
	double w;
	void put_pixel(int x, int y);
	std::vector<double> graph;
	std::vector<double> green;
	std::vector<double> red;
	double scale;
	
protected:
	virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

};

bool Marea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr){

	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	
	double max=0;
	unsigned int length;
	
	//get max ---v
	
	double maxr=*max_element(red.begin(),red.end());
	double minr=*min_element(red.begin(),red.end());
	if (std::abs(minr)>maxr){
		maxr=fabs(minr);}
	max=maxr;
		
	double maxg=*max_element(green.begin(),green.end());
	double ming=*min_element(green.begin(),green.end());
	if (std::abs(ming)>maxg)
		maxg=std::abs(ming);
	if (maxg>max)
		max=maxg;
		
	double maxb=*max_element(graph.begin(),graph.end());
	double minb=*min_element(graph.begin(),graph.end());
	if (std::abs(minb)>maxb)
		maxb=std::abs(minb);
	if (maxb>max)
		max=maxb;
		
	if (max==0){ 
		scale=1; 
	} else {
		scale=(height/2-6)/max;
	}
	//scaled ---
	
	//draw red ---v
	length=red.size();
	
	if (length!=0){
		cr->set_source_rgba(0.8, 0., 0., 0.6); 
		
		
		for (unsigned int i=0;i<length;++i){
			cr->arc(5+i*(width-10)/(length-1.01), height/2-scale*red[i], 5.0, 0.0, 2.0 * M_PI);
			cr->stroke();
		}
	}
	//draw red ---
	
	//draw green ---v
	
	length=green.size();
	
	if (length!=0){
		cr->set_source_rgba(0.0, 0.8, 0., 0.6); 
		
		for (unsigned int i=0;i<length;++i){
			cr->arc(5+i*(width-10)/(length-1.01), height/2-scale*green[i], 3.0, 0.0, 2.0 * M_PI);
			cr->stroke();
		}
	}
	
	//draw green ---
	
	//draw blue ---v
	
	length=graph.size();

	if (length!=0){
		cr->set_source_rgba(0.0, 0.0, 0.8, 0.6); 
					
		for (unsigned int i=0;i<length;++i){
			cr->arc(5+i*(width-10)/(length-1.01), height/2-scale*graph[i], 1.0, 0.0, 2.0 * M_PI);
			cr->stroke();
		}
	}	
	//draw blue---
	
	return 0;
}

class Maindow : public Gtk::Window{
	//create brain from spinners
	//mutate brain
	//draw with brain
	
public:	
	Maindow(int nn);
	Brain* brain;
	Marea brawing;
	Gtk::Window nw;
	Maindow* md;
	Maindow* pmd;
	int numclones;
	bool verbose;
	void on_smod(int);
	void on_cmod(int,int);
	void on_excmod(int);
	void on_threshmod(int);
	void on_leakmod(int);
	void on_habmod(int);
	void on_rezmod();
	
	void on_refresh();
	void on_activate();
	void on_mutate();
	void on_signalivate();
	void on_clone();
	void on_clear();
	void on_save();
	void on_setmutations();
	
	Gtk::Button refresh,activate,mutate,mutations,clone,signalivate,clear,save;
	
	//Colorcombo---v
	
	//void on_combo_changed();

	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:

		ModelColumns()
		{ add(col_id);add(col_color); }
		Gtk::TreeModelColumn<int> col_id;
		Gtk::TreeModelColumn<Glib::ustring> col_color;
	};

	ModelColumns m_Columns;

	Gtk::ComboBox m_Combo;
	Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
	
	//Colorcombo---
	
	//Activation functions ---v
	
	void on_avf_changed();
	void on_transferf_changed(int neuron);

	class AvfColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:

		AvfColumns()
		{ add(col_id);add(col_avf); }
		Gtk::TreeModelColumn<int> col_id;
		Gtk::TreeModelColumn<Glib::ustring> col_avf;
	};

	AvfColumns avf_Columns;

	Gtk::ComboBox avf_Combo;
	Glib::RefPtr<Gtk::ListStore> avf_refTreeModel;
	
	
	std::vector<AvfColumns*> transferf_Columns;
	std::vector<Gtk::ComboBox*> transferf_Combo;
	std::vector<Glib::RefPtr<Gtk::ListStore> > transferf_refTreeModel;
	Gtk::Frame transferfframe;
	Gtk::ScrolledWindow transferfscroll;
	Gtk::Grid transferfgrid;
	
	//Activation functions ---
	
		
	Gtk::Frame m_Frame;
	Gtk::Frame cmframe,excframe,threshframe,leakframe,habframe,neurframe;
	
	Glib::RefPtr<Gtk::Adjustment> s_Adj,neuradj,rezadj;
	Gtk::SpinButton s_Spin,neurspin,rezspin;

	std::vector<Glib::RefPtr<Gtk::Adjustment> > s_Adjs,cmadj,excadj,threshadj,leakadj,habadj;
	std::vector<Gtk::SpinButton*> s_Spinners,cmspin,excspin,threshspin,leakspin,habspin;
	std::vector<std::vector<Gtk::SpinButton*> > cmspinv;
	std::vector<std::vector<Glib::RefPtr<Gtk::Adjustment> > > cmadjv;



	Gtk::ButtonBox bb,bb2;
	
	Gtk::Box m_HBox,s_HBox,s_HBox2,s_HBox3;
	Gtk::Box m_VBox,b_VBox;
	std::vector<Maindow*> clones;
	
	Gtk::ScrolledWindow m_ScrolledWindow,cmscroll,excscroll,threshscroll,leakscroll,habscroll;

	Gtk::Grid s_Grid,cmgrid,excgrid,threshgrid,leakgrid,habgrid;

	Gtk::Entry saventry;
	Gtk::Entry viscale;
	Madialog* mutationsettings;
};
Maindow::Maindow(int nn):		refresh("Refresh"),
					activate("Activate"),
					mutate("Mutate"),
					mutations("Mutations"),
					clone("Clone"),
					signalivate("Signalivate"),
					clear("Clear"),
					save("Save"),
					transferfframe("Transfer functions"),
					m_Frame("Brain"),
					cmframe("Connections"),
					excframe("Excitations"),
					threshframe("Thresholds"),
					leakframe("Leaks"),
					habframe("Habituation"),

					neuradj(Gtk::Adjustment::create(nn,1,1000,1,1,0)),
					rezadj(Gtk::Adjustment::create(300,1,100000,1,1,0)),
					neurspin(neuradj,1.0,0),
					rezspin(rezadj,1.,0),

					m_HBox(Gtk::ORIENTATION_HORIZONTAL),

					s_HBox(Gtk::ORIENTATION_HORIZONTAL),
					s_HBox2(Gtk::ORIENTATION_HORIZONTAL),
					s_HBox3(Gtk::ORIENTATION_HORIZONTAL),
					m_VBox(Gtk::ORIENTATION_VERTICAL),
					b_VBox(Gtk::ORIENTATION_VERTICAL)
					{
	set_default_size(900, 500);
	m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
	m_ScrolledWindow.set_border_width(9);
	verbose=false;
	numclones=0;
	
	//Combocolor---v
	
	m_refTreeModel = Gtk::ListStore::create(m_Columns);
	m_Combo.set_model(m_refTreeModel);
	
	Gtk::TreeModel::Row row = *(m_refTreeModel->append());
	row[m_Columns.col_id] = 1;
	row[m_Columns.col_color] = "Red";
	m_Combo.set_active(row);

	row = *(m_refTreeModel->append());
	row[m_Columns.col_id] = 2;
	row[m_Columns.col_color] = "Green";

	row = *(m_refTreeModel->append());
	row[m_Columns.col_id] = 3;
	row[m_Columns.col_color] = "Blue";

	m_Combo.pack_start(m_Columns.col_color);
	
	//Combocolor---
	
	//avf---v
	
	avf_refTreeModel = Gtk::ListStore::create(avf_Columns);
	avf_Combo.set_model(avf_refTreeModel);
	
	row = *(avf_refTreeModel->append());
	row[avf_Columns.col_id] = 1;
	row[avf_Columns.col_avf] = "Linear";

	row = *(avf_refTreeModel->append());
	row[avf_Columns.col_id] = 2;
	row[avf_Columns.col_avf] = "x/(|x|+1)";
	avf_Combo.set_active(row);
	
	row = *(avf_refTreeModel->append());
	row[avf_Columns.col_id] = 3;
	row[avf_Columns.col_avf] = "Tanh";

	avf_Combo.pack_start(avf_Columns.col_avf);
	avf_Combo.set_tooltip_text("Changes transferfunction for all neurons.");
	avf_Combo.signal_changed().connect( sigc::mem_fun(*this, &Maindow::on_avf_changed) );
	
	//avf---
	
	//transferf---v
	for(int neuron=0;neuron<nn;++neuron){
		transferf_Columns.push_back(new AvfColumns);
		transferf_refTreeModel.push_back(Gtk::ListStore::create(*transferf_Columns[neuron]));
		transferf_Combo.push_back(new Gtk::ComboBox);
		transferf_Combo[neuron]->set_model(transferf_refTreeModel[neuron]);
		
		row = *(transferf_refTreeModel[neuron]->append());
		row[transferf_Columns[neuron]->col_id] = 1;
		row[transferf_Columns[neuron]->col_avf] = "Linear";

		row = *(transferf_refTreeModel[neuron]->append());
		row[transferf_Columns[neuron]->col_id] = 2;
		row[transferf_Columns[neuron]->col_avf] = "x/(|x|+1)";
		transferf_Combo[neuron]->set_active(row);
		
		row = *(transferf_refTreeModel[neuron]->append());
		row[transferf_Columns[neuron]->col_id] = 3;
		row[transferf_Columns[neuron]->col_avf] = "Tanh";

		transferf_Combo[neuron]->pack_start(transferf_Columns[neuron]->col_avf);
		transferf_Combo[neuron]->signal_changed().connect( sigc::bind<int>(sigc::mem_fun(*this, &Maindow::on_transferf_changed),neuron) );
		
		transferfgrid.attach(*transferf_Combo[neuron], 0, neuron, 1, 1);
	}
	//transferf---
	
	brain=new Brain(1,nn,1,false);
	mutationsettings=new Madialog(nn,brain);
	brain->get_inl().begin()->connect(brain->get_nl()[0]);
	brain->get_nl()[0].excite(0.1);
	int n=0;
	for (auto nit=brain->get_nl().begin();nit!=brain->get_nl().end();++nit){
		++n;
		int a=0;
		for (auto nit2=brain->get_buf().begin();nit2!=brain->get_buf().end();++nit2){
			++a;
			if (n+1==a && n!=nn)
				nit->connect(*(nit2),0.1);
			else
				nit->connect(*(nit2),0.0);
		}
	}
	
	
	int in=-1;
	int ia;
	for (auto nit=brain->get_nl().begin();nit!=brain->get_nl().end();++nit){

		++in;
		excadj.push_back(Gtk::Adjustment::create(nit->get_exc(), -1000.000, 1000.000, 0.1, 100.0, 0.0));
		threshadj.push_back(Gtk::Adjustment::create(nit->get_thresh(), -1000.000, 1000.000, 0.1, 100.0, 0.0));
		leakadj.push_back(Gtk::Adjustment::create(nit->get_leak(), -1000.000, 1000.000, 0.1, 100.0, 0.0));
		habadj.push_back(Gtk::Adjustment::create(nit->get_habituation(), -1000.000, 1000.000, 0.1, 100.0, 0.0));
		excspin.push_back(new Gtk::SpinButton(excadj[in],0.3,5));
		threshspin.push_back(new Gtk::SpinButton(threshadj[in],0.3,3));
		leakspin.push_back(new Gtk::SpinButton(leakadj[in],0.3,3));
		habspin.push_back(new Gtk::SpinButton(habadj[in],0.3,3));
		excadj[in]->signal_value_changed().connect( sigc::bind<int>(sigc::mem_fun(*this, &Maindow::on_excmod),in) );
		threshadj[in]->signal_value_changed().connect( sigc::bind<int>(sigc::mem_fun(*this, &Maindow::on_threshmod),in) );
		leakadj[in]->signal_value_changed().connect( sigc::bind<int>(sigc::mem_fun(*this, &Maindow::on_leakmod),in) );
		habadj[in]->signal_value_changed().connect( sigc::bind<int>(sigc::mem_fun(*this, &Maindow::on_habmod),in) );
		excgrid.attach(*excspin[in], 1, in, 1, 1);
		threshgrid.attach(*threshspin[in], 1, in, 1, 1);
		leakgrid.attach(*leakspin[in], 1, in, 1, 1);		
		habgrid.attach(*habspin[in], 1, in, 1, 1);		
		ia=-1;
		for (auto ait=nit->get_axv().begin();ait!=nit->get_axv().end();++ait){
			++ia;
			cmadjv.push_back(*(new std::vector<Glib::RefPtr<Gtk::Adjustment> >));
			cmadjv[in].push_back(Gtk::Adjustment::create(ait->get_cond(), -1000.000, 1000.000, 0.1, 100.0, 0.0));
			cmspinv.push_back(*(new std::vector<Gtk::SpinButton*>));
			cmspinv[in].push_back(new Gtk::SpinButton(*(cmadjv[in].begin()+ia),0.3,3));
			(*(cmadjv[in].begin()+ia))->signal_value_changed().connect( sigc::bind<int>(sigc::mem_fun(*this, &Maindow::on_cmod),in,ia) );
			cmgrid.attach(*cmspinv[in][ia], ia+1, in+1, 1, 1);
		}
		
	}
	
	int buttonvscale=29;
	int buttonhscale=131;
	cmscroll.set_size_request((ia+1)*buttonhscale,(in+1)*buttonvscale);
	excscroll.set_size_request(buttonhscale+15,(in+1)*buttonvscale);
	threshscroll.set_size_request(buttonhscale,(in+1)*buttonvscale);
	leakscroll.set_size_request(buttonhscale,(in+1)*buttonvscale);
	habscroll.set_size_request(buttonhscale,(in+1)*buttonvscale);
	transferfscroll.set_size_request(0,(in+1)*(5+buttonvscale));

	add(m_VBox);
	
	m_VBox.pack_start(m_HBox);
	s_HBox.pack_start(s_Grid,Gtk::PACK_EXPAND_WIDGET);
	
	excscroll.add(excgrid);
	excframe.add(excscroll);
	s_HBox.pack_start(excframe,Gtk::PACK_EXPAND_WIDGET);
	cmscroll.add(cmgrid);
	cmframe.add(cmscroll);
	s_HBox.pack_start(cmframe,Gtk::PACK_EXPAND_WIDGET);
	
	
	b_VBox.add(s_HBox);
	b_VBox.add(s_HBox2);
	b_VBox.add(s_HBox3);
	s_HBox2.pack_start(threshframe,Gtk::PACK_SHRINK);
	threshframe.add(threshscroll);
	threshscroll.add(threshgrid);
	
	s_HBox2.pack_start(leakframe,Gtk::PACK_SHRINK);
	leakframe.add(leakscroll);
	leakscroll.add(leakgrid);
	
	s_HBox2.pack_start(habframe,Gtk::PACK_SHRINK);
	habframe.add(habscroll);
	habscroll.add(habgrid);
	
	s_HBox2.pack_start(transferfframe,Gtk::PACK_SHRINK);
	transferfframe.add(transferfscroll);
	transferfscroll.add(transferfgrid);
	
	m_ScrolledWindow.add(b_VBox);
	
	m_Frame.add(m_ScrolledWindow);
	m_HBox.pack_start(m_Frame);
	m_HBox.pack_start(brawing,Gtk::PACK_EXPAND_WIDGET);
	
	saventry.set_text("filename.txt");

	bb.add(activate);
	activate.set_tooltip_text("Process one timestep.");
	bb.add(mutate);
	mutate.set_tooltip_text("Introduce random change.");
	bb.add(mutations);
	mutations.set_tooltip_text("Specify degree of mutation for components.");
	
	bb.add(signalivate);
	signalivate.set_tooltip_text("Feed a signal with resolution specified in right box.");
	bb.add(rezspin);
	
	bb.add(clone);
	clone.set_tooltip_text("Neuronpopulation is specified in right box.");
	bb.add(neurspin);
	bb.add(avf_Combo);
	
	//bb2.add(save);
	save.set_tooltip_text("Save netspecification.");
	//bb2.add(saventry);
	bb2.add(refresh);
	refresh.set_tooltip_text("Mmmmh, fresh.");
	//bb2.add(viscale);
	viscale.set_tooltip_text("Graphscale.");
	bb2.add(clear);
	clear.set_tooltip_text("Erase current color.");
	bb2.add(m_Combo);

	m_VBox.pack_start(bb,Gtk::PACK_SHRINK);
	m_VBox.pack_start(bb2,Gtk::PACK_SHRINK);

	refresh.signal_clicked().connect( sigc::mem_fun(*this,
              &Maindow::on_refresh) );
	activate.signal_clicked().connect( sigc::mem_fun(*this, &Maindow::on_activate) );
	mutate.signal_clicked().connect( sigc::mem_fun(*this, &Maindow::on_mutate) );
	mutations.signal_clicked().connect( sigc::mem_fun(*this, &Maindow::on_setmutations) );
    signalivate.signal_clicked().connect( sigc::mem_fun(*this, &Maindow::on_signalivate) );         
    clone.signal_clicked().connect( sigc::mem_fun(*this, &Maindow::on_clone));  
    clear.signal_clicked().connect( sigc::mem_fun(*this, &Maindow::on_clear) );   
    save.signal_clicked().connect( sigc::mem_fun(*this, &Maindow::on_save) );         

	s_Adjs.push_back(Gtk::Adjustment::create(.9, -1000.000, 1000.000, 0.1, 100.0, 0.0));
	s_Spinners.push_back(new Gtk::SpinButton(s_Adjs[s_Adjs.size()-1],0.3,3));
	(*s_Adjs.begin())->signal_value_changed().connect( sigc::bind<int>(sigc::mem_fun(*this,
              &Maindow::on_smod),1) );

	show_all_children();
}
void Maindow::on_activate(){
	brain->activate();
	switch((*m_Combo.get_active())[m_Columns.col_id]){
		case 1:
			brawing.red.push_back((brain->get_nl().end()-1)->get_exc());
			break;
		case 2:
			brawing.green.push_back((brain->get_nl().end()-1)->get_exc());
			break;
		case 3:
			brawing.graph.push_back((brain->get_nl().end()-1)->get_exc());
			break;
		default:
			std::cout<<" Error updating graph. ";
			break;
	}
	on_refresh();
}
void Maindow::on_signalivate(){
	int samples=rezspin.get_value();
	for (int i=0;i<samples;++i){
		brain->get_nl().begin()->excite(cos(i*2*M_PI/samples));
		brain->activate();
		switch((*m_Combo.get_active())[m_Columns.col_id]){
			case 1:
				brawing.red.push_back((brain->get_nl().end()-1)->get_exc());
				break;
			case 2:
				brawing.green.push_back((brain->get_nl().end()-1)->get_exc());
				break;
			case 3:
				brawing.graph.push_back((brain->get_nl().end()-1)->get_exc());
				break;
			default:
				break;
		}
	}
	on_refresh();
}
void Maindow::on_mutate(){
	brain->mutate();
	on_refresh();
}
void Maindow::on_clear(){
	switch((*m_Combo.get_active())[m_Columns.col_id]){
		case 1:
			brawing.red.clear();
			break;
		case 2:
			brawing.green.clear();
			break;
		case 3:
			brawing.graph.clear();
			break;
		default:
			break;
	}

	brawing.queue_draw();
	for (auto it=excspin.begin(); it!=excspin.end(); ++it){
		(*it)->set_value(0);
	}
}
void Maindow::on_clone(){
	if (++numclones>5)
		return;

	Maindow* mdn=new Maindow(neurspin.get_value());	
	mdn->md=md;
	md=mdn;
	md->numclones=numclones;
	md->brawing.graph.push_back(0);
	md->brawing.red.push_back(0);
	md->brawing.green.push_back(0);
	md->numclones=numclones;
	md->pmd=this;

	md->show();

	int nax;
	std::vector<Neuron>::iterator nit=brain->get_nl().begin();
	
	if (brain->get_nl().size()>md->brain->get_nl().size()){
		nax=md->brain->get_nl().size();
	} else {
		nax=brain->get_nl().size();
	}
	for (int nn=0;nn<nax;++nn){
		for (int na=0;na<nax;++na){
			md->cmspinv[nn][na]->set_value(((nit+nn)->get_axv().begin()+na)->get_cond());
		}
	}
	
}
void Maindow::on_refresh(){
	std::vector<Neuron>::iterator nit=brain->get_nl().begin();
	for (unsigned int in=0;in<brain->get_nl().size();++in){
		excspin[in]->set_value((nit+in)->get_exc());
		threshspin[in]->set_value((nit+in)->get_thresh());
		leakspin[in]->set_value((nit+in)->get_leak());
		habspin[in]->set_value((nit+in)->get_habituation());
		for (unsigned int ia=0;ia<(brain->get_nl().begin()+in)->get_axv().size();++ia){
				cmspinv[in][ia]->set_value( (brain->get_nl().begin()+in) ->get_ax(ia) ->get_cond());
		}
	}
	if (verbose){brain->print();}
	viscale.set_text(std::to_string(brawing.scale));
	brawing.queue_draw();
}
void Maindow::on_excmod(int sn){
	(brain->get_nl().begin()+sn)->set_exc(excspin[sn]->get_value());
}
void Maindow::on_smod(int sn){
	brain->get_nl().begin()->get_ax(sn)->set_cond(s_Spinners[sn-1]->get_value());
}
void Maindow::on_threshmod(int sn){
	(brain->get_nl().begin()+sn)->set_thresh(threshspin[sn]->get_value());
}
void Maindow::on_cmod(int sn,int sa){
	(brain->get_nl().begin()+sn)->get_ax(sa)->set_cond(cmspinv[sn][sa]->get_value());
}
void Maindow::on_leakmod(int sn){
	(brain->get_nl().begin()+sn)->set_leak(leakspin[sn]->get_value());
}
void Maindow::on_habmod(int sn){
	(brain->get_nl().begin()+sn)->set_habituation(habspin[sn]->get_value());
}
void Maindow::on_save(){
	brain->save(saventry.get_text());
}
void Maindow::on_avf_changed(){
	for (auto it=brain->get_nl().begin();it!=brain->get_nl().end();++it){
		switch ((*avf_Combo.get_active())[m_Columns.col_id]){
			case 1:
				it->set_trfnc(new LinearFunction(1,1));
				break;
			case 2:
				it->set_trfnc(new TailFunction(1,1));
				break;
			case 3:
				it->set_trfnc(new TanhFunction(1,1));
				break;
			default:
				break;
		}
	}
}
void Maindow::on_transferf_changed(int neuron){
	auto it=brain->get_nl().begin()+neuron;
	switch ((*(*transferf_Combo[neuron]).get_active())[m_Columns.col_id]){
		case 1:
			it->set_trfnc(new LinearFunction(1,1));
			break;
		case 2:
			it->set_trfnc(new TailFunction(1,1));
			break;
		case 3:
			it->set_trfnc(new TanhFunction(1,1));
			break;
		default:
			break;
	}
}
void Maindow::on_setmutations(){
	mutationsettings->show();
}
	
#endif
