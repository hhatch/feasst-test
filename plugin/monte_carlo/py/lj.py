import feasst

def lj():
    potential = feasst.Potential()
    potential.set_model(feasst.MakeModelLJ())
    return potential

def lj_cell():
    potential = lj()
    potential.set_visit_model(feasst.VisitModelCell())
    return potential

def lrc():
    potential = feasst.Potential()
    potential.set_visit_model(feasst.MakeLongRangeCorrections())
    return potential

def system(box_length, num=0, forcefield='data.lj'):
    system = feasst.System()
    config = feasst.Configuration(feasst.args(
        {"cubic_box_length": str(box_length),
         "particle_type": feasst.install_dir() + '/forcefield/' + forcefield,
         "init_cells": "3"}))
    for i in range(num): config.add_particle_of_type(0)
    system.add(config)
    system.add(lj())
    system.add(lrc())
    if system.configuration().domain().is_cell_enabled():
        system.add_to_optimized(lj_cell())
        system.add_to_optimized(lrc())
    return system

def add_analysis(mc, steps_per, proc="", log=""):
    mc.add(feasst.MakeLog(feasst.args(
        {"steps_per" : str(steps_per),
         "file_name": str(log)})))
    mc.add(feasst.MakeMovie(feasst.args(
        {"steps_per" : str(steps_per),
         "file_name" : "movie"+str(proc)+".xyz"})))
    mc.add(feasst.MakeCheckEnergy(feasst.args(
        {"steps_per" : str(steps_per),
         "tolerance" : "1e-8"})))
    mc.add(feasst.MakeTuner(feasst.args(
        {"steps_per" : str(steps_per)})))