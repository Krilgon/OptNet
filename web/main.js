const width = window.innerWidth;
const height = window.innerHeight;

const svg = d3.select("#graph");

const gLinks = svg.append("g");
const gBgPackets = svg.append("g");   // 🔹 фоновые пакеты
const gPathPackets = svg.append("g"); // 🔹 пакеты ГА
const gNodes = svg.append("g");

const ICONS = {
    PC: "assets/pc.svg",
    GATEWAY: "assets/gateway.svg",
    COMPUTE: "assets/compute.svg",
    STORAGE: "assets/storage.svg",
    SERVER: "assets/server.svg"
};

const COLORS = {
    PC: "#93c5fd",
    GATEWAY: "#fde047",
    COMPUTE: "#86efac",
    STORAGE: "#d1d5db",
    SERVER: "#fca5a5"
};

let pathEdges = [];

Promise.all([
    fetch("../results/input_graph.json").then(r => r.json()),
    fetch("../results/best_path.json").then(r => r.json())
]).then(([graph, bestPath]) => {
    render(graph, bestPath.path);
});

function render(graph, bestPath) {

    const nodes = Object.values(graph.nodes);
    const links = graph.edges.map(e => ({
        source: e.node_a,
        target: e.node_b
    }));

    const simulation = d3.forceSimulation(nodes)
        .force("link", d3.forceLink(links).id(d => d.id).distance(120))
        .force("charge", d3.forceManyBody().strength(-280))
        .force("center", d3.forceCenter(width / 2, height / 2));

    const link = gLinks.selectAll(".link")
        .data(links)
        .enter()
        .append("line")
        .attr("class", "link");

    const node = gNodes.selectAll(".node")
        .data(nodes)
        .enter()
        .append("g");

    node.append("circle")
        .attr("r", 20)
        .attr("fill", d => COLORS[d.type])
        .attr("stroke", "#374151");

    node.append("image")
        .attr("href", d => ICONS[d.type])
        .attr("x", -12)
        .attr("y", -12)
        .attr("width", 24)
        .attr("height", 24);

    node.append("text")
        .attr("class", "node-label")
        .attr("y", 36)
        .text(d => `${d.type}-${d.id}`);

    simulation.on("tick", () => {
        link
            .attr("x1", d => d.source.x)
            .attr("y1", d => d.source.y)
            .attr("x2", d => d.target.x)
            .attr("y2", d => d.target.y);

        node.attr("transform", d => `translate(${d.x},${d.y})`);
    });

    // 🔹 фоновая анимация по ВСЕМ рёбрам
    startBackgroundTraffic(link);

    preparePath(bestPath, link);
}

/* =========================
   BACKGROUND PACKETS
========================= */

function startBackgroundTraffic(linkSelection) {
    linkSelection.each(function () {
        spawnBgPacket(this);
    });
}

function spawnBgPacket(edge) {
    const length = edge.getTotalLength();

    const packet = gBgPackets.append("circle")
        .attr("r", 2)
        .attr("class", "bg-packet");

    packet
        .transition()
        .duration(3000 + Math.random() * 3000)
        .ease(d3.easeLinear)
        .attrTween("transform", () => t => {
            const p = edge.getPointAtLength(t * length);
            return `translate(${p.x},${p.y})`;
        })
        .on("end", () => {
            packet.remove();
            setTimeout(() => spawnBgPacket(edge), Math.random() * 1000);
        });
}

/* =========================
   PATH ANIMATION (GA)
========================= */

function preparePath(path, linkSelection) {
    pathEdges = [];

    linkSelection.each(function (d) {
        for (let i = 0; i < path.length - 1; i++) {
            if (
                (d.source.id === path[i] && d.target.id === path[i + 1]) ||
                (d.source.id === path[i + 1] && d.target.id === path[i])
            ) {
                pathEdges.push(this);
            }
        }
    });

    document.getElementById("runPath").onclick = runPathAnimation;
}

function runPathAnimation() {
    gPathPackets.selectAll("*").remove();
    d3.selectAll(".link").classed("active", false);

    pathEdges.forEach((edge, i) => {
        animatePathPacket(edge, i * 400);
    });
}

function animatePathPacket(edge, delay) {
    const length = edge.getTotalLength();

    const packet = gPathPackets.append("circle")
        .attr("r", 4)
        .attr("class", "path-packet");

    packet
        .transition()
        .delay(delay)
        .duration(1000)
        .ease(d3.easeLinear)
        .attrTween("transform", () => t => {
            const p = edge.getPointAtLength(t * length);
            return `translate(${p.x},${p.y})`;
        })
        .on("end", () => {
            d3.select(edge).classed("active", true);
            packet.remove();
        });
}
