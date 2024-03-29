var stats = { "name": "", "freemem": -1, "admin": 0, "WiFiClient_SSID": "", "WiFiClient_IP": "0.0.0.0", "WiFiAP_SSID": "", "WiFiAP_IP": "0.0.0.0", "version": "0.0", "connection": 0, "MQTT_STATUS": "-1"};

function rest(type, url, body, oncomplete, onsuccess, onerror) {
    $.ajax({type: type, url: url, data: body, complete: function() { if(oncomplete) oncomplete(); if(url!=="stats") loadStatsAdhoc(); }, success: onsuccess, error: onerror, contentType: 'text/plain'});
}

function loadStats() {
    loadStatsAdhoc(function() {if($('#autorefresh').is(':checked')) setTimeout(loadStats, 1000);});
}

function loadStatsAdhoc(loopfunc) {
    rest('GET','stats',null,
        function() {updateStatsView(); if(loopfunc) loopfunc(); }, 
        function(result) {stats = $.extend(stats, result, { "connection": 1 });},
        function(x,a,t) {stats.connection = 0; stats.admin = 0;message(t,800);}
    );
}

function formatTimeDate(seconds) {
    var now = new Date(); 
    var dt = new Date(seconds*1000 - now.getTimezoneOffset()*60000);
    try {
        if (dt.getFullYear()<1980) return "";
        return dt.toISOString().replace('T',' ').replace('.000Z','');
    } catch { 
        return ""; 
    }
}

function formatTimeShort(time) {
    if (typeof time === 'undefined') return "inf.";
    time = time / 100; // seconds
    if (time > 42949670) return "inf.";
    if (time > 172800) return Math.round(time/86400).toString() + " days left";
    if (time > 7200) return Math.round(time/3600).toString() + " hours left";
    if (time > 120) return Math.round(time/60).toString() + " mins left";
    if (time > 2) return Math.round(time).toString() + " secs left";
    return "<2 secs left";
}

function formatInt(num, len) { return ('0'.repeat(len)+num).slice(-len);}

function formatTimeLong(time) {
    if (typeof time === 'undefined') return "no changes planned";
    time = time / 100; // seconds
    if (time > 42949670) return "no changes planned";
    return (time > 86400 ? (Math.floor(time/86400) + ' day'+time>=172800 ? 's':'') : '') + // days
        ' '+formatInt(Math.floor(time/3600)%86400,2)+':'+formatInt(Math.floor(time/60)%3600,2)+':'+formatInt(Math.floor(time)%60,2);
}

var admin = false;
function updateStatsView() {
    $('#name').text(stats.name);
    $('#version').text(stats.version);
    $('#freemem').text(stats.freemem);
    $('#adminmode').text(stats.admin === 1 ? "Yes" : "No");
    $('#connected').html(stats.connection === 0 ? "<span style='color:red'>No</span>" : "<span style='color:lightgreen'>OK</span>");
    $('#mqtt').html(stats.MQTT_STATUS == "-1" ? "<span style='color:cyan'>N/A</span>" : stats.MQTT_STATUS == "0" ? "<span style='color:orange'>Connecting</span>" : "<span style='color:lightgreen'>OK</span>");
    $('#WiFiAPSSID').text(stats.WiFiAP_SSID);
    $('#WiFiAPIP').text(stats.WiFiAP_IP);
    $('#WiFiAPState').text(stats.WiFiAP_IP !== '0.0.0.0' && stats.WiFiAP_IP !== '(IP unset)' ? "enabled" : "disabled");
    $('#WiFiClientSSID').text(stats.WiFiClient_SSID);
    $('#WiFiClientIP').text(stats.WiFiClient_IP);
    $('#WiFiClientState').text((stats.WiFiClient_IP !== '0.0.0.0' && stats.WiFiClient_IP !== '(IP unset)' ? "enabled" : "disabled") + " (status: " + stats.WiFiClient_STATUS + ")");
    $('#WiFiClientChannel').text(stats.WiFiClient_CHANNEL);
    $('#WiFiClientRSSI').text(stats.WiFiClient_RSSI);
    $('#time').text(formatTimeDate(stats.time));
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
            var tstr = formatTimeShort(pin.timeleft);
            var html = tmpl.html()
                .replace(/\$type\$/g, pin.type)
                .replace(/\$typecss\$/g, pin.type.replace('/',''))
                .replace(/\$name\$/g, pin.name)
                .replace(/\$value\$/g, pin.value)
                .replace(/\$step\$/g, tstr==='inf.'?"":pin.seqstep.toString())
                .replace(/\$time\$/g, tstr)
                .replace(/\$timetooltip\$/g, formatTimeLong(pin.timeleft));
            current.replaceWith(html);
            current = $('#'+id);
            current[0].pin = pin;
            if (pin.type==='ON/OFF') current.click(clickONOFF);
        }
    }
    $('#page-connect input').attr('disabled', stats.admin !== 1);
    $('#page-boot input').attr('disabled', stats.admin !== 1);
    $(stats.admin===0 ? '#pnlLogin' : '#pnlLogoff').show();
    $(stats.admin===1 ? '#pnlLogin' : '#pnlLogoff').hide();
    
    if (!admin && stats.admin) { // fresh logon
        loadBootFile();
    }
    admin = stats.admin;
}

function clickONOFF(obj) {
    var pin = obj.currentTarget.pin;
    rest('PUT','run','SEQ '+pin.name+'\nVALUE '+(pin.value==='ON'?'OFF':'ON')+'\nEND\n');
}

function showPage(page) {
    $('#body').children('div').hide();
    $('#body #'+page).show();
    $('#body #'+page).trigger('onshow');
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
    $('#btnRestart').click(function() {restart();});
    $('#btnBootLoad').click(function() {loadBootFile();});
    $('#btnBootSave').click(function() {saveBootFile();});
    $('#page-scripts').bind('onshow', function() {loadDirScripts();});
    $('#page-wwwroot').bind('onshow', function() {loadDirWww();});
    $('#btnScriptSave').click(function() {saveScript();});
    $('#btnScriptDelete').click(function() {deleteFile(null, loadDirScripts);});
    $('#btnScriptRun').click(function() {runScript();});
    $('#btnFileUpload').click(function() {$('#fileUpload').trigger('click');});
    $('#fileUpload').change(function () {
        $('#filename').val((_folder.length>0?_folder+'/':'')+$(this).val().split('\\').pop());
        $.ajax({ url: 'upload', type: 'POST', data: new FormData($('#formUpload')[0]), cache: false, contentType: false, processData: false, success: loadDirWww });
    });
    $('#autorefresh').change(function() {
        if(this.checked) loadStats();
    });
}

function switchAP(enabled) {
    rest('PUT','run', 'WIFIAP ' + (enabled ? "1" : "0"), null, function(x) { message(x); });
}

function wifi(enabled) {
    var ssid = enabled ? $('#wifiSSID').val()+' ' : ' ';
    var pwd = enabled ? $('#wifiPwd').val()+' ' : ' ';
    var chn = enabled ? $('#wifiChn').val() : ' ';
    
    rest('PUT','run', 'WIFI ' + ssid + pwd + chn, null, function(x) { message(x); });
}

function login() {
    rest('PUT','run', 'LOGIN '+$('#loginPwd').val(), null, function(x) { message(x); });
    $('#loginPwd').val('');
}

function logoff() {
    rest('PUT','run', 'LOGOFF', null, function(x) { message(x); $('textarea').val(''); });
}

function restart() {
    rest('PUT','run', 'RESTART');
}

function loadBootFile() {
    $('#txtBoot').attr('disabled', true);
    rest('GET','boot', null, 
        function() {$('#txtBoot').attr('disabled', false);},
        function(result) { $('#txtBoot').val(result); }
    );
}

function saveBootFile() {
    rest('PUT','boot', $('#txtBoot').val(), null, function(x) { message(x); });
}

var _folder="";
var _file="";

function loadDirScripts() {
    _folder = '/scripts';
    var c = $('#files-scripts');
    c.text('');
    rest('GET','dir'+_folder, null, null, function(list) {
        for (var name in list) {
            c.append('<li onclick="loadScript(\''+name+'\')">'+name+'<span>['+list[name].toString()+']</span></li>')
        }
    });
}

function loadDirWww() {
    _folder = '';
    var c = $('#files-www');
    c.text('');
    rest('GET','dir'+_folder, null, null, function(list) {
        for (var name in list) {
            c.append('<a class="delete" href="javascript:deleteFile(\''+name+'\', loadDirWww);">[DEL]</a><li onclick="window.open(\''+name+'\')">'+name+'<span>['+list[name].toString()+']</span></li>')
        }
    });
}

function loadScript(file) {
    $('#txtScriptName').val(file); 
    rest('GET',_folder+'/'+file, null, null, function(x) { 
        $('#txtScript').val(x); 
        _file = file;
    });
}

function runScript() {
    rest('PUT', 'run', $('#txtScript').val(), null, function(x) { message(x); })
}

function saveScript() {
    _file = $('#txtScriptName').val();
    rest('PUT',_folder+'/'+_file, $('#txtScript').val(), null, function(x) { message(x); loadDirScripts(); });
}


function deleteFile(file, callback) {
    if (!confirm('Deleting file '+name+'. Do you want to continue?')) return;
    if (file) _file=file;
    rest('DELETE',_folder+'/'+_file, null, null, function(x) { message(x); if(callback) callback()});
}

function message(msg, delay=2000) {
    $('#messages').append('<div>'+msg+'</div>');
    var x = $('#messages').children().last()
    setTimeout(function() {x.remove();}, delay);
}

(function() {
    $('textarea').prop('spellcheck',false);
    showPage('page-stats');
    setTimeout(loadStats, 100);
    initButtons();
    updateStatsView();
})();