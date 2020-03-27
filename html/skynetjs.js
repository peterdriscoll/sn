var home = 'http://127.0.0.1/';
var app = angular.module('skynetApp', ['ngSanitize', 'cfp.hotkeys']);

app.controller('commandCtrl', function ($scope, $log, $sce, $http, $timeout, $window, $document, hotkeys) {
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

    // Default settings
    $scope.stepcount = '';
    $scope.maxstackframes = 10;
    $scope.maxcode = 0;
    $scope.maxderivation = 0;
    $scope.maxlog = 0;

    // Protect against requesting data again before data is retrieved.
    $scope.scheduled = 0;

    // Request JSON data sets from the server.
    $scope.initFirst = function () {
        if ($scope.closing) {
        } else if (!$scope.scheduled) {
            $scope.scheduled = $scope.scheduled + 1;
            $http.get(home + 'dashboardjs?threadnum=' + $scope.threadnum)
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
                    if ($scope.maxstackframes == -1) {
                        $scope.maxstackframes = response.data.maxstackframes;
                    }

                    $scope.scheduled = $scope.scheduled - 1;

                    if ($scope.closing) {
                        $timeout(function () { $scope.initFirst(); }, 5000);
                    } else if ($scope.running) {
                        $timeout(function () { $scope.initFirst(); }, 2000);
                    };
                });
            $http.get(home + 'stackjs?threadnum=' + $scope.threadnum + '&maxstackframes=' + $scope.maxstackframes)
                .then(function (response) {
                    $scope.frames = response.data.records;
                    $scope.callstack = [];
                    for (var i = 0; i < $scope.frames.length; i++) {
                        if ($scope.frames[i].hascode) {
                            $scope.callstack.push($scope.frames[i]);
                        }
                    }
                });
            $http.get(home + 'derivationjs?threadnum=' + $scope.threadnum + '&maxlogentries=' + $scope.maxderivation)
                .then(function (response) { $scope.derivationhtml = response.data.derivationhtml; });
            $http.get(home + 'logjs?threadnum=' + $scope.threadnum + '&maxlogentries=' + $scope.maxlog)
                .then(function (response) { $scope.logs = response.data.records; });
            $http.get(home + 'logexpjs?threadnum=' + $scope.threadnum + '&maxcode=' + $scope.maxcode)
                .then(function (response) { $scope.logexps = response.data.records; });
            $http.get(home + "stepcountjs")
                .then(function (response) { $scope.stepcounts = response.data.records; });
            $http.get(home + 'errorjs?threadnum=' + $scope.threadnum + '&maxlogentries=' + $scope.maxcode)
                .then(function (response) { $scope.error = response.data; });
            $http.get(home + 'delayedjs?threadnum=' + $scope.threadnum)
                .then(function (response) { $scope.delayedcalls = response.data.records; });
            $http.get(home + 'worldsetsjs?threadnum=' + $scope.threadnum)
                .then(function (response) { $scope.worldsets = response.data.records; });
        };
    };

    // Request an action from the server.
    $scope.submit = function (action) {
        $http.get(home + action + '?threadnum=' + $scope.threadnum + '&stackdepth=' + $scope.stackdepth + '&debugstop=' + $scope.debugstop.value + '&breakpoints=' + Array.from($scope.breakpointCol).join(','))
            .then(function (response) { $scope.initFirst(); });
    };

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
        if (threadnum != newthreadnum) {
            $scope.threadnum = newthreadnum;
            $scope.initFirst();
        }
    };

    // Open/close the goto drop down. On closing, request the server to run to the step count.
    $scope.gotostepcount = function () {
        $http.get(home + 'gotostepcountjs?threadnum=' + $scope.threadnum + '&stepcount=' + $scope.stepcount)
            .then(function (response) { $scope.initFirst(); });
    };

    $scope.updatereruncommand = function () {
        $scope.reruncommand = ($scope.laststepcount < $scope.stepcount) && ($scope.stepcount < $scope.currentstepcount);
    };
            

    // Open/close the settings drop down.
    $scope.settings = function () {
        $scope.initFirst();
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
    }
             
    // Add or remove the breakpoint from the current breakpoint button
    // From the set of breakpoint button. This also highlights or dehighlights
    // buttons in the same group.
    $scope.breakpointCol = new Set();
    $scope.breakpointGroupCol = new Map();

    $scope.getGroupBreakpoint = function (breakpointCode) {
        var count = $scope.breakpointGroupCol.get(breakpointCode);
        if (count == undefined) {
            count = 0;
        };
        return count > 0
    }

    $scope.setbreakpoint = function (breakpoint) {
        var breakpointId = breakpoint[0] + "_" + breakpoint[1];
        var breakpointCode = breakpoint[0];
        var count = $scope.breakpointGroupCol.get(breakpointCode);
        if (count == undefined) {
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
        if (breakpoint == undefined || $scope.breakpoint == undefined) {
            return "error";
        }
        var breakpointId = breakpoint[0] + "_" + breakpoint[1];
        if ((breakpoint[0] == $scope.breakpoint[0]) && (breakpoint[1] == $scope.breakpoint[1])) {
            if ($scope.breakpointCol.has(breakpointId)) {
                return 'breakpointsetat';
            }
            return 'breakpointat';
        }
        var eclass = 'breakpointclear';
        if ($scope.frames !== undefined) {
            $scope.frames.every(function (f) {
                if ((breakpoint[0] == f.breakpoint[0]) && (breakpoint[1] == f.breakpoint[1])) {
                    if ($scope.breakpointCol.has(breakpointId)) {
                        eclass = 'breakpointsetatstack';
                    } else {
                        eclass = 'breakpointatstack';
                    }
                }
                return eclass == 'breakpointclear';
            });
        }
        if (eclass == 'breakpointclear') {
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

    $scope.selectedvar = '';
    $scope.selectvar = function (name) {
        if ($scope.selectedvar == name)
        {
            $scope.selectedvar = '';
        }
        else
        {
            $scope.selectedvar = name;
        }
    }
    $scope.selecttext = function (name, text) {
        if ($scope.selectedvar == name)
        {
            return text;
        }
        else
        {
            return name;
        }
    }

    // Initial request for data.
    $scope.initFirst();
})
.directive('initBind', function ($compile) {
    return {
        restrict: 'A',
        link: function (scope, element, attr) {
            attr.$observe('ngBindHtml', function () {
                if (attr.ngBindHtml) {
                    $compile(element[0].children)(scope);
                }
            })
        }
    };
});