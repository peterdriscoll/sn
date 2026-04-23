var home = window.location.origin + '/';
var app = angular.module('skynetApp', ['ngSanitize', 'cfp.hotkeys']);

// in skynetjs.js, after angular.module('skynetApp', [...])
angular.module('skynetApp')
    .run(['$http', '$interval', '$window', function ($http, $interval, $window) {
        var PERIOD_MS = 1000, TIMEOUT_MS = 1500, CLOSE_AFTER_MISSES = 3;
        var misses = 0;

        function tryClose() {
            try {
                if ($window.opener) $window.close();      // works if opened by opener.html
                else $window.location.replace('about:blank'); // fallback
            } catch (e) { $window.location.replace('about:blank'); }
        }

        function ping() {
            $http.get(home + 'healthz', { cache: false, timeout: TIMEOUT_MS })
                .then(function () { misses = 0; })
                .catch(function () {
                    if (++misses >= CLOSE_AFTER_MISSES) {
                        $interval.cancel(timer);
                        tryClose();
                    }
                });
        }

        var timer = $interval(ping, PERIOD_MS);
    }]);

app.controller('commandCtrl', function ($scope, $log, $sce, $http, $timeout, $interval, $window, $document, hotkeys) {
    $scope.trustAsHtml = $sce.trustAsHtml;
    $scope.threadnum = 0;
    $scope.currentstepcount = 0;
    $scope.laststepcount = 0;

    $scope.debugstopoptions = [
        { name: 'none', value: '0' },
        { name: 'end', value: '1' },
        { name: 'task', value: '2' },
        { name: 'error', value: '3' },
        { name: 'warning', value: '4' },
        { name: 'info', value: '5' },
        { name: 'debug', value: '6' },
        { name: 'detail', value: '7' }
    ];

    $scope.debugstop = $scope.debugstopoptions[6];

    $scope.stepcounts = { "threadnum": "0", "stepcount": "1" };
    
    // Default settings
    $scope.stepcount = 0;
    $scope.maxderivation = 30;
    $scope.errorstepcount = 0;

    // Call stack frames
    $scope.maxcallstackframes = 4;
    $scope.bufcallstackframes = 2;
    $scope.startcallstackframes = 0;
    $scope.callstackstepcount = 0;
    $scope.displayedcallstackframes = 0;

    // Stack frames
    $scope.maxstackframes = 30;
    $scope.bufstackframes = 5;
    $scope.startstackframes = 0;
    $scope.stackstepcount = 0;
    $scope.displayedstackframes = 0;

    // Code history
    $scope.maxcode = 30;
    $scope.bufcode = 10;
    $scope.startcode = 0;
    $scope.codestepcount = 0;
    $scope.displayedcode = 0;

    // Log
    $scope.maxlog = 30;
    $scope.buflog = 10;
    $scope.startlog = 0;
    $scope.logstepcount = 0;
    $scope.displayedlogentries = 0;

    $scope.countcalls = 0;
    $scope.countframes = 0;
    $scope.countdelayedcalls = 0;
    $scope.isDelayedCallsOpen = false;
    $scope.countworldsets = 0;
    $scope.isWorldSetsOpen = false;
    $scope.countlogentries = 0;
    $scope.countcodeentries = 0;

    // Protect against requesting data again before data is retrieved.
    $scope.scheduled = 0;

    $scope.isRequestPending = false;

    $scope.worldSetMap = {};
    $scope.isMapReady = false;
    $scope.isGotoOpen = false; // Drawer is closed by default
    // Load the error history.
    $scope.loadthreadstepcounts = function () {
        $http.get(home + "stepcount.json" + '?time=' + new Date().getTime())
            .then(function (response) {
                $scope.stepcounts = response.data.records;
            }).catch(function (err) {
                console.error("Stepcount first load failed:", err);
            }); 
    };

    // Load the error history.
    $scope.loaderrors = function () {
        $scope.errorstepcount = $scope.currentstepcount;
        $http.get(home + 'error.json?threadnum=' + $scope.threadnum + '&maxlogentries=' + $scope.maxcode + '&time=' + new Date().getTime())
            .then(function (response) {
                $scope.error = response.data;
                if ($scope.errorstepcount === $scope.currentstepcount) {
                    $scope.loadsecondarydata();
                }
            });
    };

    // Load the world sets, if the detail for it is open.
    $scope.loadworldsets = function (isToggle) {
        // If it's a manual click, flip the state first
        if (isToggle) {
            $scope.isWorldSetsOpen = !$scope.isWorldSetsOpen;
        }

        // Only fetch if it's now open (either from startup or the toggle)
        if ($scope.isWorldSetsOpen) {
            $http.get(home + 'worldsets.json?threadnum=' + $scope.threadnum + '&time=' + new Date().getTime())
                .then(function (response) {
                    // 1. Get the records safely
                    var records = (response.data && response.data.records) ? response.data.records : [];
                    $scope.worldsets = records;

                    // 2. Clear and rebuild the map
                    $scope.worldSetMap = {};

                    records.forEach(function (ws) {
                        if (ws && ws.id) {
                            $scope.worldSetMap[ws.id] = ws;
                        }
                    });

                    $scope.isMapReady = true;
                    console.log("Map Build Complete. Keys found:", Object.keys($scope.worldSetMap).length);
                })
                .catch(function (err) {
                    console.error("Failed to load worldsets:", err);
                });
        }
    };

    // Load the delayed calls, if the detail for it is open.
    $scope.loaddelayedcalls = function (isToggle) {
        // 1. Flip the state if it's a user click
        if (isToggle) {
            $scope.isDelayedCallsOpen = !$scope.isDelayedCallsOpen;
        }

        // 2. Only fetch if it's open (handles startup call and manual toggle)
        if ($scope.isDelayedCallsOpen) {
            $http.get(home + 'delayed.json?threadnum=' + $scope.threadnum + '&time=' + new Date().getTime())
                .then(function (response) {
                    $scope.delayedcalls = response.data.records;
                });
        }
    };

    // Load the stack if the detail for it is open.
    $scope.loadstack = function (opening) {
        var field = $document[0].getElementById('stackid');
        if ($scope.stackstepcount !== $scope.currentstepcount) {
            $scope.stackstepcount = $scope.currentstepcount;
            $scope.startstackframes = 0;
            $scope.displayedstackframes = 0;
        }
        if (opening && !field.open || !opening && field.open) {
            if ($scope.startstackframes < $scope.maxstackframes && $scope.startstackframes < $scope.countframes) {
                $http.get(home + 'stack.json?threadnum=' + $scope.threadnum + '&maxstackframes=' + $scope.bufstackframes + '&startstackframes=' + $scope.startstackframes + "&stepcount=" + $scope.stackstepcount + '&time=' + new Date().getTime())
                    .then(function (response) {
                        if ($scope.stackstepcount === $scope.currentstepcount && $scope.stackstepcount == response.data.stepcount) {
                            if ($scope.startstackframes === 0) {
                                $scope.displayedstackframes = 0;
                                $scope.frames = response.data.records;
                            } else {
                                for (var i = 0; i < response.data.records.length; i++) {
                                    $scope.frames.push(response.data.records[i]);
                                }
                            }
                            $scope.displayedstackframes += response.data.records.length;
                            $scope.startstackframes += $scope.bufstackframes;
                            if ($scope.startstackframes < $scope.maxstackframes && $scope.startstackframes < $scope.countframes) {
                                $scope.loadstack(false);
                            }
                        }
                    });
            }
        }
    };

    // Load the call stack if the detail for it is open.
    $scope.loadcallstack = function (opening) {
        if ($scope.countcalls == 0)
        {
            $scope.callstack = {};
            $scope.displayedcallstackframes = 0;
        }
        else
        {
            var field = $document[0].getElementById('callstackid');
            if ($scope.callstackstepcount !== $scope.currentstepcount) {
                $scope.callstackstepcount = $scope.currentstepcount;
                $scope.startcallstackframes = 0;
                $scope.displayedcallstackframes = 0;
            }
            if (opening && !field.open || !opening && field.open) {
                if ($scope.startcallstackframes < $scope.maxcallstackframes && $scope.startcallstackframes < $scope.countcalls) {
                    $http.get(home + 'callstack.json?threadnum=' + $scope.threadnum + '&maxcallstackframes=' + $scope.bufcallstackframes + '&startcallstackframes=' + $scope.startcallstackframes + "&stepcount=" + $scope.callstackstepcount + '&time=' + new Date().getTime())
                        .then(function (response) {
                            if ($scope.callstackstepcount === $scope.currentstepcount && $scope.callstackstepcount == response.data.stepcount) {
                                if ($scope.startcallstackframes === 0) {
                                    $scope.displayedcallstackframes = 0;
                                    $scope.callstack = response.data.records;
                                } else {
                                    for (var i = 0; i < response.data.records.length; i++) {
                                        $scope.callstack.push(response.data.records[i]);
                                    }
                                }
                                $scope.displayedcallstackframes += response.data.records.length;
                                $scope.startcallstackframes += $scope.bufcallstackframes;
                                if ($scope.startcallstackframes < $scope.maxcallstackframes && $scope.startcallstackframes < $scope.countcalls) {
                                    $scope.loadcallstack(false);
                                }
                            }
                        });
                }
            }
        }
    };

    // Load the call stack if the detail for it is open.
    $scope.loadwatchlist = function (opening) {
        var field = $document[0].getElementById('watchlistid');
        if (opening && !field.open || !opening && field.open) {
            $http.get(home + 'watchlist.json?threadnum=' + $scope.threadnum + '&time=' + new Date().getTime())
                .then(function (response) { $scope.watchlist = response.data.records; });
        }
    };

    // Load the derivation interpretation of the log, if the detail for it is open.
    $scope.loadderivations = function (opening) {
        var field = $document[0].getElementById('derivationsid');
        if (opening && !field.open || !opening && field.open) {
            $http.get(home + 'derivation.json?threadnum=' + $scope.threadnum + '&maxlogentries=' + $scope.maxderivation + '&time=' + new Date().getTime())
                .then(function (response) { $scope.derivationhtml = response.data.derivationhtml; });
        }
    };

    // Load the log  of code expressions, if the detail for it is open.
    $scope.loadcode = function (opening) {
        var field = $document[0].getElementById('codeid');
        if ($scope.codestepcount !== $scope.currentstepcount) {
            $scope.codestepcount = $scope.currentstepcount;
            $scope.startcode = 0;
            $scope.displayedcode = 0;
        }
        if (opening && !field.open || !opening && field.open) {
            if ($scope.startcode < $scope.maxcode && $scope.startcode < $scope.countcodeentries) {
                $http.get(home + 'code.json?threadnum=' + $scope.threadnum + '&maxcode=' + $scope.bufcode + '&startcode=' + $scope.startcode + "&stepcount=" + $scope.codestepcount + '&time=' + new Date().getTime())
                    .then(function (response) {
                        if ($scope.codestepcount === $scope.currentstepcount && $scope.codestepcount === response.data.stepcount) {
                            if ($scope.startcode === 0) {
                                $scope.displayedcode = 0;
                                $scope.code = response.data.records;
                            } else {
                                for (var i = 0; i < response.data.records.length; i++) {
                                    $scope.code.push(response.data.records[i]);
                                }
                            }
                            $scope.displayedcode += response.data.records.length;
                            $scope.startcode += $scope.bufcode;
                            if ($scope.startcode < $scope.maxcode && $scope.startcode < $scope.countcodeentries) {
                                $scope.loadcode(false);
                            }
                        }
                    });
            }
        }
    };

    // Load the log  of code expressions, if the detail for it is open.
    $scope.loadlog = function (opening) {
        var field = $document[0].getElementById('logid');
        if ($scope.logstepcount !== $scope.currentstepcount) {
            $scope.logstepcount = $scope.currentstepcount;
            $scope.startlog = 0;
            $scope.displayedlogentries = 0;
        }
        if (opening && !field.open || !opening && field.open) {
            if ($scope.startlog < $scope.maxlog && $scope.startlog < $scope.countlogentries) {
                $http.get(home + 'log.json?threadnum=' + $scope.threadnum + '&maxlogentries=' + $scope.buflog + '&startlog=' + $scope.startlog + "&stepcount=" + $scope.logstepcount + '&time=' + new Date().getTime())
                    .then(function (response) {
                        if ($scope.logstepcount === $scope.currentstepcount && $scope.logstepcount === response.data.stepcount) {
                            if ($scope.startlog === 0) {
                                $scope.displayedlogentries = 0;
                                $scope.logs = response.data.records;
                            } else {
                                for (var i = 0; i < response.data.records.length; i++) {
                                    $scope.logs.push(response.data.records[i]);
                                }
                            }
                            $scope.displayedlogentries += response.data.records.length;
                            $scope.startlog += $scope.buflog;
                            if ($scope.startlog < $scope.maxlog && $scope.startlog < $scope.countlogentries) {
                                $scope.loadlog(false);
                            }
                        }
                    });
            }
        }
    };

    // Request JSON data sets from the server.
    $scope.loaddata = function () {
        if (!$scope.closing) {
            $http.get(home + 'dashboard.json?threadnum=' + $scope.threadnum + '&time=' + new Date().getTime())
                .then(function (response) {
                    $scope.threadnum = response.data.threadnum;
                    $scope.breakpoint = response.data.breakpoint;
                    $scope.taskdescription = response.data.taskdescription;
                    $scope.breakpointdescription = response.data.breakpointdescription;
                    $scope.statusdescription = response.data.statusdescription;
                    $scope.running = response.data.running;
                    $scope.closing = response.data.closing;
                    $scope.currentstepcount = response.data.currentstepcount;
                    $scope.laststepcount = response.data.laststepcount;
                    $scope.countcalls = response.data.countcalls;
                    $scope.countframes = response.data.countframes;
                    $scope.countdelayedcalls = response.data.countdelayedcalls;
                    $scope.countworldsets = response.data.countworldsets;
                    $scope.countlogentries = response.data.countlogentries;
                    $scope.countcodeentries = response.data.countcodeentries;

                    if ($scope.maxstackframes === -1) {
                        $scope.maxstackframes = response.data.maxstackframes;
                    }

                    $scope.scheduled = $scope.scheduled - 1;

                    $scope.loaderrors();
                    $scope.loadcallstack(false);
                    $scope.loadthreadstepcounts();
                }).catch(function (err) {
                    $log.error("Main load failed", err);
                }).finally(function () {
                    // 4. Schedule the next heartbeat
                    if ($scope.isrunning) {
                        $timeout($scope.loaddata, 2000);
                    }
                });
        }
    };

    // Request JSON secondary data sets from the server. These are less immediate.
    $scope.loadsecondarydata = function () {
        $scope.loadstack(false);
        $scope.loadderivations(false);
        $scope.loadcode(false);
        $scope.loadlog(false);
        $scope.loaddelayedcalls(false);
        $scope.loadworldsets(false);
        $scope.loadwatchlist(false);
    };

    // Request an action from the server.
    $scope.submit = function (action) {
        $http.get(home + action + '?threadnum=' + $scope.threadnum + '&stackdepth=' + $scope.stackdepth + '&debugstop=' + $scope.debugstop.value + '&breakpoints=' + Array.from($scope.breakpointCol).join(',') + '&time=' + new Date().getTime())
            .then(function (response) {
                if (action === "runtoendjs") {
                    $timeout(function () {
                    window.close();
                    }, 1000);
                } else {
                    $scope.loaddata();
                }
            });
    };

    $scope.openGotoDrawer = function () {
        $scope.isGotoOpen = true;

        // We use a small timeout (350ms) to wait for the CSS slide 
        // animation to finish before grabbing focus.
        setTimeout(function () {
            var input = document.getElementById('stepcountid');
            if (input) {
                input.focus();
                input.select();
            }
        }, 350);
    };

    $scope.closeGotoDrawer = function () {
        $scope.isGotoOpen = false;
    };

    $scope.heartbeat = function () {
        if ($scope.isRequestPending || !$scope.isrunning) return;

        $scope.isRequestPending = true;

        $scope.loaddata();
    };

    // 3. Start a steady 2-second interval
    $interval($scope.heartbeat, 2000);

    hotkeys.add({
        combo: 'd',
        description: 'Run to breakpoint',
        callback: function () {
            $scope.submit("debugjs");
        }
    });

    hotkeys.add({
        combo: 'ctrl+f5',
        description: 'Run to breakpoint',
        callback: function () {
            $scope.submit("debugjs");
        }
    });

    hotkeys.add({
        combo: 'r',
        description: 'Run',
        callback: function () {
            $scope.submit("runjs");
        }
    });

    hotkeys.add({
        combo: 'e',
        description: 'Run to end',
        callback: function () {
            $scope.submit("runtoendjs");
        }
    });

    hotkeys.add({
        combo: 'c',
        description: 'Code debug break C++',
        callback: function () {
            $scope.submit("codebreakjs");
        }
    });

    hotkeys.add({
        combo: 'v',
        description: 'Step over',
        callback: function () {
            $scope.submit("stepoverjs");
        }
    });

    hotkeys.add({
        combo: 'f10',
        description: 'Step over',
        callback: function () {
            $scope.submit("stepoverjs");
        }
    });

    hotkeys.add({
        combo: 'i',
        description: 'Step into',
        callback: function () {
            $scope.submit("stepintojs");
        }
    });

    hotkeys.add({
        combo: 'ctrl+f11',
        description: 'Step into',
        callback: function () {
            $scope.submit("stepintojs");
        }
    });

    hotkeys.add({
        combo: 'o',
        description: 'Step out of call',
        callback: function () {
            $scope.submit("stepoutjs");
        }
    });

    hotkeys.add({
        combo: 'O',
        description: 'Step out of func',
        callback: function () {
            $scope.submit("stepoutcalljs");
        }
    });

    hotkeys.add({
        combo: 'shift+f11',
        description: 'Step out of func',
        callback: function () {
            $scope.submit("stepoutcalljs");
        }
    });

    hotkeys.add({
        combo: 'g',
        description: 'Goto step count',
        callback: function () {
            $scope.gotostepcount();
        }
    });

    hotkeys.add({
        combo: 's',
        description: 'Dashboard settings',
        callback: function () {
            $scope.settings();
        }
    });

    // Switch to a different thread.
    $scope.setthread = function (newthreadnum) {
        if (threadnum !== newthreadnum) {
            $scope.threadnum = newthreadnum;
            $scope.loaddata();
        }
    };

    // Open/close the goto drop down. On closing, request the server to run to the step count.
    $scope.gotostepcount = function () {
        $http.get(home + 'gotostepcountjs?threadnum=' + $scope.threadnum + '&stepcount=' + $scope.stepcount + '&time=' + new Date().getTime())
            .then(function (response) { $scope.loaddata(); $scope.loadthreadstepcounts(); });
    };

    $scope.gotostepcountsetfocus = function () {
        // Use $timeout to ensure the dropdown has actually rendered before focusing
        $timeout(function () {
            var stepCountField = $document[0].getElementById('stepcountid');
            if (stepCountField) {
                stepCountField.focus();
                stepCountField.select(); // This highlights the "0"
            }
        }, 100); // 100ms is usually enough for the dropdown transition
    };

    // Open/close the settings drop down.
    $scope.settings = function () {
        $scope.loaddata();
    };

    $scope.gotostepcountsetfocus = function () {
        var stepCountField = $document[0].getElementById('stepcountid');
        stepCountField.focus();
    };

    $scope.reruntostackpoint = function (stepcount) {
        $scope.stepcount = stepcount;
        $scope.updatereruncommand();
        var stepCountField = $document[0].getElementById('stepcountid');
        stepCountField.value = stepcount;
        stepCountField.focus();
        stepCountField.scrollIntoView({ behavior: "smooth", block: "end", inline: "nearest" });
    };

    // Add or remove the breakpoint from the current breakpoint button
    // From the set of breakpoint button. This also highlights or dehighlights
    // buttons in the same group.
    $scope.breakpointCol = new Set();
    $scope.breakpointGroupCol = new Map();

    $scope.getGroupBreakpoint = function (breakpointCode) {
        var count = $scope.breakpointGroupCol.get(breakpointCode);
        if (count === undefined) {
            count = 0;
        }
        return count > 0;
    };

    $scope.setbreakpoint = function (breakpoint) {
        var breakpointId = breakpoint[0] + "_" + breakpoint[1];
        var breakpointCode = breakpoint[0];
        var count = $scope.breakpointGroupCol.get(breakpointCode);
        if (count === undefined) {
            count = 0;
        }
        if ($scope.breakpointCol.has(breakpointId)) {
            $scope.breakpointCol.delete(breakpointId);
            count--;
            if (count <= 0) {
                $scope.breakpointGroupCol.delete(breakpointCode);
            }
            else {
                $scope.breakpointGroupCol.set(breakpointCode, count);
            }
        }
        else {
            $scope.breakpointCol.add(breakpointId);
            count++;
            $scope.breakpointGroupCol.set(breakpointCode, count);
        }
    };

    // Set default breakpoints.
    $scope.setbreakpoint(["Error", 16]);
    $scope.setbreakpoint(["Warning", 20]);
    $scope.setbreakpoint(["Warning", 19]);
    $scope.setbreakpoint(["Warning", 23]);
    $scope.setbreakpoint(["user", 22]);
    $scope.setbreakpoint(["Assert", 3]);
    $scope.setbreakpoint(["PartialAssert", 3]);
    $scope.setbreakpoint(["Evaluate", 3]);
    $scope.setbreakpoint(["PartialEvaluate", 3]);

    // Create the breakpoint to display a breakpoint button by comparing
    // current breakpoint and call stack in the running code with the
    // breakpoint represented by the breakpoint button.
    // breakpointdefaultclass passes in a default class
    $scope.breakpointclass = function (breakpoint) {
        if (breakpoint === undefined || $scope.breakpoint === undefined) {
            return "error";
        }
        var breakpointId = breakpoint[0] + "_" + breakpoint[1];
        if (breakpoint[0] === $scope.breakpoint[0] && breakpoint[1] === $scope.breakpoint[1]) {
            if ($scope.breakpointCol.has(breakpointId)) {
                return 'breakpointsetat';
            }
            return 'breakpointat';
        }
        var eclass = 'breakpointclear';
        if ($scope.frames !== undefined) {
            $scope.frames.every(function (f) {
                if (breakpoint[0] === f.breakpoint[0] && breakpoint[1] === f.breakpoint[1]) {
                    if ($scope.breakpointCol.has(breakpointId)) {
                        eclass = 'breakpointsetatstack';
                    } else {
                        eclass = 'breakpointatstack';
                    }
                }
                return eclass === 'breakpointclear';
            });
        }
        if (eclass === 'breakpointclear') {
            if ($scope.breakpointCol.has(breakpointId)) {
                eclass = 'breakpointset';
            } else {
                var breakpointCode = breakpoint[0];
                if ($scope.getGroupBreakpoint(breakpointCode)) {
                    eclass = 'breakpointgroupset';
                }
            }
        }
        return eclass;
    };

    // Same as breakpoint class but add a default class.
    $scope.breakpointdefaultclass = function (breakpoint, c) {
        return $scope.breakpointclass(breakpoint) + ' ' + c;
    };

    $scope.watchListDisplayAll = false;
    $scope.watchList = new Set();

    $scope.setWatch = function (id) {
        if ($scope.watchList.has(id)) {
            $scope.watchList.delete(id);
        }
        else
        {
            $scope.watchList.add(id);
        }
    }
 
    $scope.getWatch = function (id) {
        return $scope.watchList.has(id);
    }
 
    $scope.watchClass = function (id) {
        if ($scope.getWatch(id))
        {
            return 'watchset'
        }
        return 'watchclear';
    };

    $scope.getWatchDisplay = function (id) {
        return $scope.watchListDisplayAll || $scope.watchList.has(id);
    }
    $scope.worldSetListDisplayAll = false;
    $scope.worldSetList = new Set();

    $scope.toggleWorldSet = function (id) {
        if ($scope.worldSetList.has(id)) {
            $scope.worldSetList.delete(id);
        }
        else {
            $scope.worldSetList.add(id);
        }
    }

    $scope.setWorldSet = function (id) {
        if (!$scope.worldSetList.has(id)) {
            $scope.worldSetList.add(id);
        }
    }

    $scope.getWorldSet = function (id) {
        return $scope.worldSetList.has(id);
    }

    $scope.getWorldSetDisplay = function (id, valueset, childWorldList) {
        return ($scope.worldSetListDisplayAll || $scope.getWorldSet(id)) && (!$scope.worldSetsFilterValueSets || valueset);
    }

    $scope.clearAllWorldSets = function (id) {
        return $scope.worldSetList.clear();
    }

    $scope.worldList = new Set();

    $scope.toggleWorld = function (id, setId) {
        if ($scope.worldList.has(id)) {
            $scope.worldList.delete(id);
        }
        else {
            $scope.worldList.add(id);
            $scope.worldSetList.add(setId);
            if ($scope.isMapReady)
            {
                $scope.clearParentMarks();
                $scope.clearChildMarks();

                var parentSetList = $scope.worldSetMap[setId].parentsets;

                console.log("Parent set", setId, Object.keys(parentSetList).length);
                parentSetList.forEach(function (worldSet) {
                    console.log("Parent set id:", worldSet.id);

                    $scope.worldSetList.add(worldSet.id);
                    $scope.setParentMark(worldSet.id);

                    var childSetList = $scope.worldSetMap[worldSet.id].childsets;
                    childSetList.forEach(function (worldSet) {
                        $scope.worldSetList.add(worldSet.id);
                    });
                });
                console.log("Current Parent Marks:", [...$scope.parentMarkSet]);
            }
        }
    }

    $scope.setWorld = function (id) {
        if (!$scope.worldList.has(id)) {
            $scope.worldList.add(id);
        }
    }

    $scope.getWorld = function (id) {
        return $scope.worldList.has(id);
    }

    $scope.worldClass = function (id) {
        if ($scope.getWorld(id)) {
            return 'worldset'
        }
        return 'worldclear';
    };

    $scope.parentMarkSet = new Set();

    $scope.setParentMark = function (id) {
        $scope.parentMarkSet.add(id);
    }

    $scope.clearParentMarks = function () {
        $scope.activeParentId = '';
        $scope.parentMarkSet.clear();
    }

    $scope.activeParentId = '';

    $scope.parentMarkClass = function (id) {
        if (id === $scope.activeParentId)
        {
            return 'activeparentmark';
        }
        if ($scope.parentMarkSet.has(id)) {
            return 'parentmark';
        }
        return '';
    };

    $scope.childMarkSet = new Set();

    $scope.setChildMark = function (id) {
        $scope.childMarkSet.add(id);
    }

    $scope.clearChildMarks = function () {
        $scope.childMarkSet.clear();
    }

    $scope.childMarkClass = function (id) {
        if ($scope.childMarkSet.has(id)) {
            return 'childmark';
        }
        return '';
    };

    $scope.markChildWorldSets = function (id, childsets) {
        $scope.worldSetList.add(id);
        $scope.activeParentId = id;

        // 1. Clear the old state (The "No-Undo" reset)
        $scope.clearChildMarks();

        // 2. Apply the new state
        if (childsets && childsets.length > 0) {
            childsets.forEach(function (childSet) {
                $scope.setChildMark(childSet.id);
                $scope.worldSetList.add(childSet.id);
            });
        }
        console.log("Current Child Marks:", [...$scope.childMarkSet]);
    };

    $scope.selectedvar = '';
    $scope.selectvar = function (name) {
        if ($scope.selectedvar === name) {
            $scope.selectedvar = '';
        }
        else {
            $scope.selectedvar = name;
        }
    };

    $scope.selecttext = function (name, text) {
        if ($scope.selectedvar === name) {
            return text;
        }
        else {
            return name;
        }
    };

    // Initial request for data.
    $scope.loaddata();
});

app.directive('initBind', function ($compile) {
    return {
        restrict: 'A',
        link: function (scope, element, attr) {
            attr.$observe('ngBindHtml', function () {
                if (attr.ngBindHtml) {
                    $compile(element[0].children)(scope);
                }
            });
        }
    };
});