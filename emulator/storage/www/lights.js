var stats = { "name": "", "freemem": -1, "admin": 0, "WiFiClient_SSID": "", "WiFiClient_IP": "0.0.0.0", "WiFiAP_SSID": "", "WiFiAP_IP": "0.0.0.0", "version": "0.0", "connection": 0};

function rest(type, url, body, callback) {
    var xhttp = new XMLHttpRequest();
    if(callback) xhttp.onreadystatechange = callback;
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
var admin = false;
function updateStatsView() {
    $('#name').text(stats.name);
    $('#version').text(stats.version);
    $('#freemem').text(stats.freemem);
    $('#adminmode').text(stats.admin === 1 ? "yes" : "no");
    $('#connection').text(stats.connection === 0 ? "disconnected" : "connected");
    $('#WiFiAPSSID').text(stats.WiFiAP_SSID);
    $('#WiFiAPIP').text(stats.WiFiAP_IP);
    $('#WiFiAPState').text(stats.WiFiAP_IP !== '0.0.0.0' ? "enabled" : "disabled");
    $('#WiFiClientSSID').text(stats.WiFiClient_SSID);
    $('#WiFiClientIP').text(stats.WiFiClient_IP);
    $('#WiFiClientState').text(stats.WiFiClient_IP !== '0.0.0.0' ? "enabled" : "disabled");
    var statsContainer = $('#stats-pins');
    var tmpl = $('#template-pin');
    if (stats.pins) {
        for(var i=0;i<stats.pins.length;i++) {
            var pin = stats.pins[i];
            var id = "pin-" + pin.name;
            var current = $('#'+id);
            if (current.length == 0) {
                statsContainer.append('<div></div>');
                current = statsContainer.children().last();
            }
            var html = tmpl.html().replace(/\$type\$/g, pin.type).replace(/\$name\$/g, pin.name).replace(/\$value\$/g, pin.value);
            current.replaceWith(html);
        }
    }
    $('#page-wifi input').attr('disabled', stats.admin !== 1);
    $('#page-boot input').attr('disabled', stats.admin !== 1);
    $(stats.admin===0 ? '#pnlLogin' : '#pnlLogoff').show();
    $(stats.admin===1 ? '#pnlLogin' : '#pnlLogoff').hide();
    
    if (!admin && stats.admin) { // fresh logon
        loadBootFile();
    }
    admin = stats.admin;
}

function showPage(page) {
    $('#body').children('div').hide();
    $('#body #'+page).show();
    $('#body #'+page).trigger('isShown');
    $('#menu ul li').removeClass('selected');
    $('#menu ul li a[page="'+page+'"]').parent().addClass('selected');
}

function initButtons() {
    $('#menu ul li a').click(function() { showPage($(this).attr('page')); });
    $('#menu ul li a').each(function() { this.href="javascript:void(0);"; });
    $('#btnWiFiAPEnable').click(function() { switchAP(true); }); 
    $('#btnWiFiAPDisable').click(function() { switchAP(false); }); 
    $('#btnWiFiConnect').click(function() { wifi(true); }); 
    $('#btnWiFiDisconnect').click(function() { wifi(false); }); 
    $('#loginPwd').keypress(function(e) {var code = e.keyCode || e.which; if(code == 13) login();})
    $('#btnLogin').click(function() {login();});
    $('#btnLogoff').click(function() {logoff();});
    //$('#body #page-boot').bind('isShown', loadBootFile);
    $('#btnBootSave').click(function() {saveBootFile();});
}

function switchAP(enabled) {
    var temp = $('#chkTemporary')[0].checked;
    rest('PUT','run', (temp ? 'WIFIAP ':'WIFIAPTEST ') + (enabled ? "1" : "0"), null);
}

function wifi(enabled) {
    var temp = $('#chkTemporary')[0].checked;
    var ssid = enabled ? $('#wifiSSID').val()+' ' : ' ';
    var pwd = enabled ? $('#wifiPwd').val() : ' ';
    rest('PUT','run', (temp ? 'WIFI ':'WIFITEST ') + ssid + pwd, null);
}

function login() {
    rest('PUT','run', 'LOGIN '+$('#loginPwd').val(), null);
    $('#loginPwd').val('');
}

function logoff() {
    rest('PUT','run', 'LOGOFF', null);
}

function loadBootFile() {
    $('#txtBoot').attr('disabled', true);
    rest('GET','boot', null, function (e) { 
        var xhr = e.currentTarget;
        if (xhr.readyState === 4) {
            if(xhr.status === 200) {
                $('#txtBoot').val(xhr.responseText);
            }
            $('#txtBoot').attr('disabled', false);
        }
    });
}

function saveBootFile() {
    rest('PUT','boot', $('#txtBoot').val(), null);
}

(function() {
    showPage('page-stats');
    setTimeout(loadStats, 100);
    initButtons();
    updateStatsView();
})();