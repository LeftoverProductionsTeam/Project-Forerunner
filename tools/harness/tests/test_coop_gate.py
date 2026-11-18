"""Co-op one-way gates: real player lists, volume tests and placement code."""
import sys
from pathlib import Path
import pytest
sys.path.insert(0,str(Path(__file__).resolve().parents[2]))
from harness import build,read,run,function,structure,constant,mutated,CHECK_FAILED

def generated(fault=None):
    lists=read('source/hs/object_lists.c')
    code='\n'.join(function(lists,n) for n in ['object_list_new','object_list_mark_players','object_list_is_players','object_list_add'])
    players=function(read('source/hs/hs_library_external.c'),'hs_players')
    port=read('port/linux/game/coop_scripts.c')
    gather='static boolean coop_scripts_adaptations;\n'+'\n'.join(function(port,n) for n in ['coop_scripts_new_game','coop_scripts_adaptations_active','coop_scripts_native_player_actions','coop_scripts_any_player_will_do','coop_scripts_empty_player_list','teleport_to','gather_player_in_volume','coop_scripts_gather_in_volume'])
    gather=function(read('source/game/players.c'),'players_coop_teleport_stacked')+'\n'+gather
    if fault=='adaptations':gather=mutated(gather,'config_boolean("network.coop_script_adaptations")','TRUE')
    gate=function(read('source/hs/hs_library_external.c'),'hs_trigger_volume_test_objects_all')
    if fault=='unverified':gate=mutated(gate,'inside = hs_trigger_volume_test_objects(trigger_volume_index, object_list_index, TRUE);','inside = TRUE;')
    if fault=='empty':players=mutated(players,'object_list_mark_players(object_list_index);',';')
    if fault=='recycled':code=mutated(code,'list->flags = 0; /* port: a recycled slot has no previous provenance */',';')
    if fault=='rollback':gather=mutated(gather,'object_set_position(unit_index, &original, NULL, NULL);',';')
    if fault=='stack':gather=mutated(gather,'attempt != 0','FALSE')
    if fault=='reference':code=mutated(code,'if (list->first_reference_index == first) return FALSE;', 'if (list->first_reference_index == first && FALSE) return FALSE;')
    vision='\n'.join(function(read('source/hs/hs_library_external.c'),n) for n in ['hs_any_player_can_see','hs_objects_can_see_object','hs_objects_can_see_flag'])
    return (('list_header.inc','#define _object_list_players_bit '+str(constant(lists,'_object_list_players_bit'))+'\n'+structure(read('source/hs/object_lists.h'),'object_list_header_datum')),('lists.inc',code),('players.inc',players),('gather.inc',gather),('volume.inc',function(read('source/hs/hs_library_external.c'),'hs_trigger_volume_test_objects')),('gate.inc',gate),('vision.inc',vision))

@pytest.mark.parametrize('case',['one-player','crowds','refused','outside','bsp-result','empty','allocation','lifetime','contexts','mixed','no-arrival','bsp-anchor','native-lists','native-visibility','map-policy','reference-exhaustion'])
def test_gate(case):
    status,output=run(build('coop_gate',generated()),case)
    assert status==0,output

@pytest.mark.parametrize('fault,case',[('unverified','refused'),('empty','empty'),('recycled','lifetime'),('rollback','outside'),('stack','crowds'),('adaptations','native-lists'),('reference','reference-exhaustion')])
def test_negative(fault,case):
    status,output=run(build('coop_gate',generated(fault)),case)
    assert status==CHECK_FAILED,output
