(function () {
    'use strict';
    angular.module('app', [
        'ngRoute',
        'ngAnimate',
        'ui.bootstrap',

        'app.controllers',
        'app.localization',
        'app.nav',
        'app.ui.services',
        'app.chat',
        'app.services',

        'mgcrea.ngStrap',
        'ngToast',
        'ngImgCrop',
        'naif.base64',
        'mc.resizer'
        //'app.services.mobile'
    ])

    .run((['$location', '$rootScope' ,function($location, $rootScope, editableOptions) {
            $rootScope.$on('$routeChangeSuccess', function (event, current, previous) {
                $rootScope.title = current.$$route.title;
            });
    }] ) )


        .filter('bytes', function() {
        return function(bytes, precision) {
            if (isNaN(parseFloat(bytes)) || !isFinite(bytes)) return '-';
            if (typeof precision === 'undefined') precision = 1;
            var units = ['bytes', 'kB', 'MB', 'GB', 'TB', 'PB'],
                number = Math.floor(Math.log(bytes) / Math.log(1024));
            return (bytes / Math.pow(1024, Math.floor(number))).toFixed(precision) +  ' ' + units[number];
        }
    })


        .config(['$routeProvider',  '$httpProvider', '$compileProvider', function ($routeProvider, $httpProvider, $compileProvider) {
            $httpProvider.interceptors.push('authHttpResponseInterceptor');
            var routes, setRoutes;

            //$httpProvider.defaults.withCredentials = true;
            //$compileProvider.aHrefSanitizationWhitelist(/^\s*(https?|ftp|mailto|file|javascript|data|http):/);
            $compileProvider.aHrefSanitizationWhitelist(/[\x21-\x7E]/);

            //$httpProvider.interceptors.push(function($q, localStorageService ) {
            //    return {
            //        'request': function(config) {
            //            //console.log(config);
            //            // same as above
            //            return config;
            //        },
            //
            //        'response': function(response) {
            //            // same as above
            //            console.log("AAAA");
            //            console.log(response);
            //            console.log("BBB");
            //
            //            if (response.status == 200 && response.config.method == "GET"){
            //                localStorageService.set(response.config.url, response);
            //            }
            //
            //
            //            if (response.status == 503 && response.config.method == "GET"){
            //                response =  localStorageService.get(response.config.url);
            //            }
            //            //503 error
            //
            //            return response;
            //
            //        }
            //    };
            //});



            routes = [
                'dashboard',
                'pages/404',
                'pages/403',
                'pages/500',
                'pages/503',
                'system/debug'
            ];


            setRoutes = function (route) {
                var config, url;
                url = '/' + route;
                config = {
                    templateUrl: 'views/' + route + '.html'
                };
                $routeProvider.when(url, config);
                return $routeProvider;
            };
            routes.forEach(function (route) {
                return setRoutes(route);
            });
            return $routeProvider.when('/', {
                redirectTo: '/dashboard'
            }).when('/404', {
                templateUrl: 'views/pages/404.html'
            }).otherwise({
                redirectTo: '/404'
            });
        }
        ])

        /// Reverse elements in array filter
        .filter('reverseCus', function() {
            return function(items, revBoolVal) {
                if (revBoolVal.revBoolVal){
                    return items.slice().reverse();
                }else{
                    return items;
                }
            };
        })

        /// directive to do some action (function) by clicking enter on the page
        .directive('ngEnter', function() {
            return function(scope, element, attrs) {
                element.bind("keydown keypress", function(event) {
                    if(event.which === 13) {
                        scope.$apply(function(){
                            scope.$eval(attrs.ngEnter, {'event': event});
                        });

                        event.preventDefault();
                    }
                });
            };
        });

}).call(this)

;

