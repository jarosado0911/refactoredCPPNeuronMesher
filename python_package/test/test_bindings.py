import pytest, os, inspect
from rich import print
from rich.table import Table
import python_package.neurongraph as ng

def get_test_data_path(filename):
    return os.path.join(os.path.dirname(__file__),filename)

def get_output_dir():
    return os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'output'))

delta = 2.0

@pytest.fixture(scope="module")
def graph():
    g = ng.NeuronGraph()
    path = get_test_data_path("neuron.swc")
    g.readFromFile(path)
    return g

def test_initializer(graph):
    fn = inspect.currentframe().f_code.co_name
    g0 = ng.NeuronGraph()
    print(f"\n[blue] TEST {fn}:[/] [green]Number of nodes:[/] {g0.numberOfNodes()}")
    assert g0.numberOfNodes()==0

def test_initializer2(graph):
    fn = inspect.currentframe().f_code.co_name
    infile= get_test_data_path("neuron.swc")
    g0 = ng.NeuronGraph(infile)
    print(f"\n[blue] TEST {fn}:[/] [green]Number of nodes:[/] {g0.numberOfNodes()}")
    assert g0.numberOfNodes()>0

def test_number_of_nodes(graph):
    fn = inspect.currentframe().f_code.co_name
    count = graph.numberOfNodes()
    print(f"\n[blue] TEST {fn}:[/] [green]Number of nodes:[/] {count}")
    assert count > 0

def test_number_of_edges(graph):
    fn = inspect.currentframe().f_code.co_name
    count = graph.numberOfEdges()
    print(f"\n[blue] TEST {fn}:[/] [green]Number of edges:[/] {count}")
    assert count > 0

def test_get_nodes(graph):
    fn = inspect.currentframe().f_code.co_name
    nodes = graph.getNodes()
    print(f"\n[blue] TEST {fn}:[/] [cyan]Got {len(nodes)} nodes[/]")
    assert isinstance(nodes, dict)
    assert all(isinstance(k, int) for k in nodes.keys())

def test_topological_sort(graph):
    fn = inspect.currentframe().f_code.co_name
    before = graph.isTopologicallySorted()
    graph.topologicalSort()
    after = graph.isTopologicallySorted()
    print(f"\n[blue] TEST {fn}:[/] [yellow]Topologically sorted before:[/] {before}, [yellow]after:[/] {after}")
    assert after is True

def test_has_soma_segment(graph):
    fn = inspect.currentframe().f_code.co_name
    hasSoma = graph.hasSomaSegment()
    print(f"\n[blue] TEST {fn}:[/] has soma segment? {hasSoma}")
    assert hasSoma is True

def test_is_soma_missing(graph):
    fn = inspect.currentframe().f_code.co_name
    print(f"\n[blue] TEST {fn}:[/] isSomaMissing? {graph.isSomaMissing()}")
    assert graph.isSomaMissing() is False

def test_remove_soma_segment(graph):
    fn = inspect.currentframe().f_code.co_name
    before = graph.getNodes()
    after = graph.removeSomaSegment()
    print(f"\n[blue] TEST {fn}:[/] [yellow] Pre #nodes:[/] {len(before)}, [yellow] Post #nodes:[/] {len(after)}")
    assert len(after) < len(before)

def test_set_nodes(graph):
    fn = inspect.currentframe().f_code.co_name
    before = graph.getNodes()
    graph.setNodes(graph.getNodes())
    after = graph.getNodes()
    print(f"\n[blue] TEST {fn}:[/] [yellow] Pre #nodes:[/] {len(before)}, [yellow] Post #nodes:[/] {len(after)}")
    assert len(after)==len(before)

def test_preprocess(graph):
    fn = inspect.currentframe().f_code.co_name
    before = graph.getNodes()
    after = graph.preprocess(before)
    print(f"\n[blue] TEST {fn}:[/] [yellow] Pre #nodes:[/] {len(before)}, [yellow] Post #nodes:[/] {len(after)}")
    assert len(after) < len(before)

def test_read_from_fileUGX(graph):
    fn = inspect.currentframe().f_code.co_name
    graph.readFromFile(get_test_data_path("neuron.swc"))
    before = graph.numberOfNodes()
    newgraph = ng.NeuronGraph()
    newgraph.readFromFileUGX(get_test_data_path("neuron.ugx"))
    after = newgraph.numberOfNodes()
    print(f"\n[blue] TEST {fn}:[/] [yellow] SWC:[/] {before}, [yellow] UGX:[/] {after}")
    assert after == before

def test_write_to_ugx(graph):
    fn = inspect.currentframe().f_code.co_name
    outfilename = get_output_dir()+"/test/writeUGX.ugx"
    graph.writeToFileUGX(outfilename)
    before = graph.numberOfNodes()
    graph.readFromFileUGX(outfilename)
    after = graph.numberOfNodes()
    print(f"\n[blue] TEST {fn}:[/] [yellow] WRT:[/] {before}, [yellow] READ:[/] {after}")
    assert after == before

def test_save_preprocess(graph):
    fn = inspect.currentframe().f_code.co_name
    outfilename = get_output_dir()+"/test/preprocessSWC.swc"
    before = graph.getNodes()
    after = graph.preprocess(before)
    graph.writeToFile(after,outfilename)
    print(f"\n[blue] TEST {fn}:[/] [yellow] Pre:[/] {len(before)}, [yellow] Post:[/] {len(after)}")
    assert len(before) > len(after)

def test_splitedges(graph):
    fn = inspect.currentframe().f_code.co_name
    outfilename = get_output_dir()+"/test/splitSWC.swc"
    before = graph.getNodes()
    after = graph.splitEdges()
    graph.writeToFile(after,outfilename)
    print(f"\n[blue] TEST {fn}:[/] [yellow] Pre:[/] {len(before)}, [yellow] Post:[/] {len(after)}")
    assert len(after) > len(before)

def test_splitedgesN(graph):
    fn = inspect.currentframe().f_code.co_name
    before = graph.getNodes()
    N = 4
    afterset = graph.splitEdgesN(N)
    for i, after in enumerate(afterset):
        outfilename = get_output_dir()+"/test/splitSWC_"+str(i)+".ugx"
        graph.writeToFileUGX(after,outfilename)
        print(f"\n[blue] TEST {fn}:[/] [yellow] Pre:[/] {len(before)}, [yellow] Post:[/] {len(after)}")
        assert len(after) >= len(before)

def test_get_trunks(graph):
    fn = inspect.currentframe().f_code.co_name
    trunks = graph.getTrunks(False)
    print(f"\n[blue] TEST {fn}:[/] [yellow] Num. Trunks:[/] {len(trunks)}")
    assert len(trunks) > 0

def test_assemble_trunks(graph):
    fn = inspect.currentframe().f_code.co_name
    trunks = graph.getTrunks(False)
    assembled = graph.assembleTrunks(trunks)
    print(f"\n[blue] TEST {fn}:[/] [yellow] #nodes:[/] {len(assembled)}")
    assert len(assembled) == graph.numberOfNodes()

    outfilename = get_output_dir()+"/test/reassembled1.swc"
    graph.writeToFile(assembled,outfilename)
    assert True

def test_assemble_trunks2(graph):
    fn = inspect.currentframe().f_code.co_name
    trunks = graph.getTrunks(False)
    trunkmap = graph.getTrunkParentMap(graph.getNodes(),trunks)
    trunks = graph.getTrunks(True)
    assembled = graph.assembleTrunks(trunks,trunkmap)
    print(f"\n[blue] TEST {fn}:[/] [yellow] #nodes:[/] {len(assembled)}")
    assert len(assembled) == graph.numberOfNodes()

    outfilename = get_output_dir()+"/test/reassembled2.swc"
    graph.writeToFile(assembled,outfilename)
    assert True

def test_assemble_linear_resample(graph):
    fn = inspect.currentframe().f_code.co_name
    trunks = graph.getTrunks(False)
    trunkmap = graph.getTrunkParentMap(graph.getNodes(),trunks)
    resampled = graph.allLinearSplineResampledTrunks(trunks,delta)
    assembled = graph.assembleTrunks(resampled,trunkmap)
    print(f"\n[blue] TEST {fn}:[/] [yellow] #nodes:[/] {len(assembled)}")
    assert len(assembled) != graph.numberOfNodes()

    outfilename = get_output_dir()+"/test/reassembledlinear.swc"
    graph.writeToFile(assembled,outfilename)
    assert True

def test_assemble_cubic_resample(graph):
    fn = inspect.currentframe().f_code.co_name
    trunks = graph.getTrunks(False)
    trunkmap = graph.getTrunkParentMap(graph.getNodes(),trunks)
    resampled = graph.allCubicSplineResampledTrunks(trunks,delta)
    assembled = graph.assembleTrunks(resampled,trunkmap)
    print(f"\n[blue] TEST {fn}:[/] [yellow] #nodes:[/] {len(assembled)}")
    assert len(assembled) != graph.numberOfNodes()

    outfilename = get_output_dir()+"/test/reassembledcubic.swc"
    graph.writeToFile(assembled,outfilename)
    assert True

def test_swc_to_ugx(graph):
    fn = inspect.currentframe().f_code.co_name
    infile= get_test_data_path("neuron.swc")
    outfile = get_output_dir()+"/test/swc2ugx.ugx"
    graph.swc2ugx(infile,outfile)
    before = graph.numberOfNodes()
    graph.readFromFileUGX(outfile)
    after = graph.numberOfNodes()
    print(f"\n[blue] TEST {fn}:[/] [yellow] SWC:[/] {before}, [yellow] UGX:[/] {after}")
    assert after == before

def test_ugx_to_swc(graph):
    fn = inspect.currentframe().f_code.co_name
    infile= get_test_data_path("neuron.ugx")
    outfile = get_output_dir()+"/test/ugx2swc.swc"
    graph.ugx2swc(infile,outfile)
    before = graph.numberOfNodes()
    graph.readFromFile(outfile)
    after = graph.numberOfNodes()
    print(f"\n[blue] TEST {fn}:[/] [yellow] UGX:[/] {before}, [yellow] SWC:[/] {after}")
    assert after == before

def test_generate_refinements_cubic(graph):
    fn = inspect.currentframe().f_code.co_name
    delta,N,method = 12.0,6,"cubic"
    before = graph.getNodes()
    refinements = graph.generateRefinements(before,delta,N,method)
    for _,after in refinements.items():
        print(f"\n[blue] TEST {fn}:[/] [yellow] before:[/] {len(before)}, [yellow] after:[/] {len(after)}")
        assert len(after) >= len(before)

def test_generate_refinements_linear(graph):
    fn = inspect.currentframe().f_code.co_name
    delta,N,method = 12.0,6,"linear"
    before = graph.getNodes()
    refinements = graph.generateRefinements(before,delta,N,method)
    for _,after in refinements.items():
        print(f"\n[blue] TEST {fn}:[/] [yellow] before:[/] {len(before)}, [yellow] after:[/] {len(after)}")
        assert len(after) >= len(before)