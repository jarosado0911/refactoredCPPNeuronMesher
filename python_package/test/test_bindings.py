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

def test_number_of_nodes(graph):
    fn = inspect.currentframe().f_code.co_name
    count = graph.numberOfNodes()
    print(f"[blue] TEST {fn}:[/] [green]Number of nodes:[/] {count}")
    assert count > 0

def test_number_of_edges(graph):
    fn = inspect.currentframe().f_code.co_name
    count = graph.numberOfEdges()
    print(f"[blue] TEST {fn}:[/] [green]Number of edges:[/] {count}")
    assert count > 0

def test_get_nodes(graph):
    fn = inspect.currentframe().f_code.co_name
    nodes = graph.getNodes()
    print(f"[blue] TEST {fn}:[/] [cyan]Got {len(nodes)} nodes[/]")
    assert isinstance(nodes, dict)
    assert all(isinstance(k, int) for k in nodes.keys())

def test_topological_sort(graph):
    fn = inspect.currentframe().f_code.co_name
    before = graph.isTopologicallySorted()
    graph.topologicalSort()
    after = graph.isTopologicallySorted()
    print(f"[blue] TEST {fn}:[/] [yellow]Topologically sorted before:[/] {before}, [yellow]after:[/] {after}")
    assert after is True

def test_has_soma_segment(graph):
    fn = inspect.currentframe().f_code.co_name
    hasSoma = graph.hasSomaSegment()
    print(f"[blue] TEST {fn}:[/] has soma segment? {hasSoma}")
    assert hasSoma is True

def test_is_soma_missing(graph):
    fn = inspect.currentframe().f_code.co_name
    print(f"[blue] TEST {fn}:[/] isSomaMissing? {graph.isSomaMissing()}")
    assert graph.isSomaMissing() is False

def test_remove_soma_segment(graph):
    fn = inspect.currentframe().f_code.co_name
    before = graph.getNodes()
    after = graph.removeSomaSegment()
    print(f"[blue] TEST {fn}:[/] [yellow] Pre #nodes:[/] {len(before)}, [yellow] Post #nodes:[/] {len(after)}")
    assert len(after) < len(before)

def test_set_nodes(graph):
    fn = inspect.currentframe().f_code.co_name
    before = graph.getNodes()
    graph.setNodes(graph.getNodes())
    after = graph.getNodes()
    print(f"[blue] TEST {fn}:[/] [yellow] Pre #nodes:[/] {len(before)}, [yellow] Post #nodes:[/] {len(after)}")
    assert len(after)==len(before)

def test_preprocess(graph):
    fn = inspect.currentframe().f_code.co_name
    before = graph.getNodes()
    after = graph.preprocess(before)
    print(f"[blue] TEST {fn}:[/] [yellow] Pre #nodes:[/] {len(before)}, [yellow] Post #nodes:[/] {len(after)}")
    assert len(after) < len(before)

def test_read_from_fileUGX(graph):
    fn = inspect.currentframe().f_code.co_name
    graph.readFromFile(get_test_data_path("neuron.swc"))
    before = graph.numberOfNodes()
    newgraph = ng.NeuronGraph()
    newgraph.readFromFileUGX(get_test_data_path("neuron.ugx"))
    after = newgraph.numberOfNodes()
    print(f"[blue] TEST {fn}:[/] [yellow] SWC:[/] {before}, [yellow] UGX:[/] {after}")
    assert after == before

def test_write_to_ugx(graph):
    fn = inspect.currentframe().f_code.co_name
    outfilename = get_output_dir()+"/test/writeUGX.ugx"
    graph.writeToFileUGX(outfilename)
    before = graph.numberOfNodes()
    graph.readFromFileUGX(outfilename)
    after = graph.numberOfNodes()
    print(f"[blue] TEST {fn}:[/] [yellow] WRT:[/] {before}, [yellow] READ:[/] {after}")
    assert after == before

def test_save_preprocess(graph):
    fn = inspect.currentframe().f_code.co_name
    outfilename = get_output_dir()+"/test/preprocessSWC.swc"
    before = graph.getNodes()
    after = graph.preprocess(before)
    graph.writeToFile(after,outfilename)
    print(f"[blue] TEST {fn}:[/] [yellow] Pre:[/] {len(before)}, [yellow] Post:[/] {len(after)}")
    assert len(before) > len(after)

def test_splitedges(graph):
    fn = inspect.currentframe().f_code.co_name
    outfilename = get_output_dir()+"/test/splitSWC.swc"
    before = graph.getNodes()
    after = graph.splitEdges()
    graph.writeToFile(after,outfilename)
    print(f"[blue] TEST {fn}:[/] [yellow] Pre:[/] {len(before)}, [yellow] Post:[/] {len(after)}")
    assert len(after) > len(before)

def test_splitedgesN(graph):
    fn = inspect.currentframe().f_code.co_name
    before = graph.getNodes()
    N = 4
    afterset = graph.splitEdgesN(N)
    for i, after in enumerate(afterset):
        outfilename = get_output_dir()+"/test/splitSWC_"+str(i)+".ugx"
        graph.writeToFileUGX(after,outfilename)
        print(f"[blue] TEST {fn}:[/] [yellow] Pre:[/] {len(before)}, [yellow] Post:[/] {len(after)}")
        assert len(after) >= len(before)

def test_get_trunks(graph):
    fn = inspect.currentframe().f_code.co_name
    trunks = graph.getTrunks(False)
    print(f"[blue] TEST {fn}:[/] [yellow] Num. Trunks:[/] {len(trunks)}")
    assert len(trunks) > 0

def test_assemble_trunks(graph):
    fn = inspect.currentframe().f_code.co_name
    trunks = graph.getTrunks(False)
    assembled = graph.assembleTrunks(trunks)
    print(f"[blue] TEST {fn}:[/] [yellow] #nodes:[/] {len(assembled)}")
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
    print(f"[blue] TEST {fn}:[/] [yellow] #nodes:[/] {len(assembled)}")
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
    print(f"[blue] TEST {fn}:[/] [yellow] #nodes:[/] {len(assembled)}")
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
    print(f"[blue] TEST {fn}:[/] [yellow] #nodes:[/] {len(assembled)}")
    assert len(assembled) != graph.numberOfNodes()

    outfilename = get_output_dir()+"/test/reassembledcubic.swc"
    graph.writeToFile(assembled,outfilename)
    assert True



    