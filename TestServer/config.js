let config = {};

// The port which calls from Unreal Engine plugin clients will call to access RPC commands indirectly and other things.
config.hostname = "95.216.164.23";
config.host_port = 6668;

config.websocket = {}
config.websocket.host_port = 6667;
config.websocket.all_conf_host_port = 6669;

config.node = {};
// Websocket address + port of the node
config.node.ws_address = "ws://[::1]:7078"

// Wallet which can be used for requesting Nano, like a faucet
config.node.wallet = "5C9B72D16ACD64FFD49E420CF4CF962895C57FA679C589045B3B34C7CD98E59B";
config.node.source = "nano_191o3sb8a3qhnnxcmrbc1y7urei73e3z7m9cg3bzwz4u5j4m43j9p76pwkcc"; 
config.node.request_raw_amount = "1000000000000000000";

// The RPC server settings for communicating with the node
config.node.rpc = {};
config.node.rpc.address = "[::1]";
config.node.rpc.port = 7076;

config.dpow = {};
config.dpow.enabled = false;
config.dpow.user = "<your_username>";
config.dpow.api_key = "<your_api_key>";
config.dpow.ws_address = "wss://dpow.nanocenter.org/service_ws/";

module.exports = config;
