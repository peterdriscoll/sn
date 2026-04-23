var home = 'http://127.0.0.1/';
var app = angular.module('testinteligenteditingApp', ['ngSanitize']);

// in skynetjs.js, after angular.module('skynetApp', [...])
angular.module('testinteligenteditingApp')
    .run(['$http', '$interval', '$window', function ($http, $interval, $window) {
        var PERIOD_MS = 5000, TIMEOUT_MS = 1500, CLOSE_AFTER_MISSES = 3;
        var misses = 0;
    }]);

app.controller('commandCtrl', function ($scope, $log, $sce, $http, $timeout, $interval, $window, $document) {
    $scope.editTarget = "";

    $scope.assertion = {
        type: 'Assert',
        content: {
            type: 'IsA',
            subject: { name: 'Waggy', type: 'leaf' },
            predicate: { name: 'Dog', type: 'leaf' }
        }
    };

    // This variable will now hold a reference to the actual object being edited
    $scope.activePiece = null;

    $scope.getFullLogicString = function (piece) {
        if (!piece) return "";

        // If it's a leaf node (Waggy, Dog), just return its name
        if (piece.type === 'leaf') {
            return piece.name;
        }

        // If it's the IsA structure
        if (piece.type === 'IsA') {
            return $scope.getFullLogicString(piece.subject) + ".IsA(" +
                $scope.getFullLogicString(piece.predicate) + ")";
        }

        // If it's the Assert structure (the root)
        if (piece.type === 'Assert') {
            return $scope.getFullLogicString(piece.content) + ".Assert()";
        }

        return piece.name || "";
    };

    $scope.editLogicPiece = function (piece, $event) {
        if ($event) $event.stopPropagation();
        $scope.activePiece = piece;

        // Populate the text box with the full string for this level
        $scope.editTarget = $scope.getFullLogicString(piece);
    };
});