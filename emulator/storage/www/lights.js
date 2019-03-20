var stats = { "name": "", "freemem": -1, "admin": 0, "WiFiClient_SSID": "", "WiFiClient_IP": "0.0.0.0", "WiFiAP_SSID": "", "WiFiAP_IP": "0.0.0.0", "version": "0.0", "connection": 0};

function rest(type, url, body, callback) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = callback;
    xhttp.open(type, url, true);
    xhttp.send(body);
}

function loadStats() {
    //$.ajax({
    //    type: 'GET', url: 'stats', data: null,
    //    complete: function () { setTimeout(loadStats, 1000); },
    //    success: function (text) { stats = $.extend(stats, eval(text), { "connection": 1 }); updateStatsView(); },
    //    error: function () { stats.connection = 0; updateStatsView(); }
    //});
    rest('GET', 'stats', null, function (e) {
        var xhr = e.currentTarget;
        if (xhr.readyState === 4 ) {
            if (xhr.status === 200) {
                eval("var x=" + xhr.responseText);
                stats = $.extend(stats, x, { "connection": 1 });
                updateStatsView();
            } else if (xhr.status > 400) {
                stats.connection = 0; 
                updateStatsView();
            }
            setTimeout(loadStats, 1000);
        }
    });
}

function updateStatsView() {
    $('#name').text(stats.name);
    $('#version').text(stats.version);
    $('#freemem').text(stats.freemem);
    $('#adminmode').text(stats.admin === 1 ? "yes" : "no");
    $('#connection').text(stats.connection === 0 ? "disconnected" : "connected");
    $('#WiFiAPSSID').text(stats.WiFiAP_SSID);
    $('#WiFiAPIP').text(stats.WiFiAP_IP);
    $('#WiFiAPState').text(stats.WiFiAP_IP !== '0.0.0.0' ? "enabled" : "disabled");
    $('#wifiClientSSID').text(stats.WiFiClient_SSID);
    $('#WiFiClientIP').text(stats.WiFiClient_IP);
    $('#WiFiClientState').text(stats.WiFiClient_IP !== '0.0.0.0' ? "enabled" : "disabled");
}

function showPage(page) {
    $('#body').children('div').hide();
    $('#body #'+page).show();
    $('#menu ul li').removeClass('selected');
    $('#menu ul li a[page="'+page+'"]').parent().addClass('selected');
}

function initButtons() {
    $('#menu ul li a').click(function() { showPage($(this).attr('page')); });
    $('#menu ul li a').each(function() { this.href="javascript:void(0);"; });
}

(function() {
    showPage('page-stats');
    setTimeout(loadStats, 100);
    initButtons();
    updateStatsView();
})();