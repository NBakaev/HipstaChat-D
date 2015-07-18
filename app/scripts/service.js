'use strict';

angular.module('app.services', ['restangular', 'LocalStorageModule' ])
    .service('userInfo', function (Restangular, localStorageService, $http, configurationService,$q, $aside,$rootScope ) {
        Restangular.setBaseUrl(configurationService.returnAPIhost());
        Restangular.setDefaultHeaders({"Accept": "application/json"});
        Restangular.setDefaultHeaders({"Content-Type": "application/json"});

        var myAccount = {};
        this.getCurrentUser = function () {
            var abc = myAccount.allInfo = Restangular.one('myaccount').get();
            abc.then(function (e) {
                abc = e;
            });

            return abc;
        };

        if (window.hipstachat && window.hipstachat.mobile){

            $rootScope.currentRoomsFast = $aside({scope: $rootScope, html:true,placement:"right", show:false, animation:"am-slide-right",  contentTemplate: 'templates/fastRoom.html'});
            $rootScope.currentRoomsFast.showAction = function() {
                $rootScope.currentRoomsFast.$promise.then( $rootScope.currentRoomsFast.show);
            };

            (function () {
                var count = 0;
                //Enable swiping...
                $("html").swipe({
                    //Generic swipe handler for all directions
                    swipeLeft: function (event, direction, distance, duration, fingerCount) {

                        //console.log($rootScope.currentRoomsFast.isShown);


                        $rootScope.currentRoomsFast.hide();

                        //if ($rootScope.currentRoomsFast.show){
                        //    $rootScope.currentRoomsFast.hide();
                        //}



                        // свайп влево и меню навигацции показано - закрываем
                        if ($('#app').hasClass('on-canvas')) {
                            $('#app').toggleClass('on-canvas');
                        } else {

                            $rootScope.currentRoomsFast.showAction();
                        }
                        //                $(this).text("You swiped " + direction + " " + ++count + " times " );
                    },

                    swipeRight: function (event, direction, distance, duration, fingerCount) {

                        // если свайп вправо и нет показано мею навигацци - показываем
                        if (!$('#app').hasClass('on-canvas')) {
                            $('#app').toggleClass('on-canvas');
                        }else{
                            //$rootScope.currentRoomsFast.hide();
                        }


                        //$('#app').toggleClass('on-canvas');
                        //                $(this).text("You swiped " + direction + " " + ++count + " times " );
                    },

                    //Default is 75px, set to 0 for demo so any distance triggers swipe
                    threshold: 75,
                    cancelable: true


                })

            })();
        }




    })










    .factory('favicoService', [
        function() {
            var favico = new Favico({
                animation : 'fade'
            });

            var badge = function(num) {
                favico.badge(num);
            };
            var reset = function() {
                favico.reset();
            };

            return {
                badge : badge,
                reset : reset
            };
        }])

            .factory('authHttpResponseInterceptor', ['$q', '$location', function ($q, $location) {
                return {
                    response: function (response) {
                        // ERROR INTERCEPT
                        if (response.status === 401) {
                            //$location.path('login/login');
                            window.location = "http://hipstachat.tk/accounts/login/";
                            console.log("Response 401");
                        }

                        if (response.status === 401) {
                            $location.path('pages/403');
                            console.log("Response 403");
                        }

                        //if (response.status === 503) {
                        //    $location.path('pages/503');
                        //    console.log("Response 503");
                        //}

                        return response || $q.when(response);
                    },
                    responseError: function (rejection) {
                        if (rejection.status === 401) {
                            console.log("Response Error 401", rejection);
                            window.location = "/accounts/login/";
                            //$location.path('login/login').search('returnTo', $location.path());
                        }

                        if (rejection.status === 403) {
                            console.log("Response Error 403", rejection);
                            $location.path('pages/403').search('returnTo', $location.path());
                        }

                        //if (rejection.status === 503) {
                        //    console.log("Response Error 503", rejection);
                        //    $location.path('pages/503').search('returnTo', $location.path());
                        //}
                        return $q.reject(rejection);
                    }
                }
            }])

            .service('configurationService', function (Restangular, localStorageService, $http, $location) {//
                this.returnAPIhost = function () {

                    function printSD() {
                        window.apiServerDefined = true;
                        window.loading_screen.finish(); // убираем экран загрузки

                        console.log("------------------------------");
                        console.log("       API SERVER ADDRESS");
                        console.log("Address: " + apiURL);
                    }


                        var apiURL = "http://192.168.10.132:33333/";

                    printSD();
                    return apiURL;
                };


                this.onlineStatus = {
                    "connectedToStaticServer": true,
                    "connectedToApiServer": true
                };

                this.getOnlineStatusResulted = function () {
                    return this.onlineStatus;
                };

                function httpRequest(address, reqType, asyncProc) {
                    var r = window.XMLHttpRequest ? new XMLHttpRequest() : new ActiveXObject("Microsoft.XMLHTTP");
                    if (asyncProc)
                        r.onreadystatechange = function () {
                            if (this.readyState == 4) asyncProc(this);
                        };

                    r.open(reqType, address, !(!asyncProc));
                    r.onerror = function () {
                    };
                    try {
                        r.send();
                    }
                    catch (e) {
                    }
                    return r;
                }

                this.getOnlineStatus = function () {
                    var returnAPIhost = this.returnAPIhost;
                };

                this.getOnlineStatus();


                console.info("API: " + this.getOnlineStatusResulted().connectedToApiServer);
                console.info("------------------------------");
                console.info("STATIC: " + this.getOnlineStatusResulted().connectedToStaticServer);
            })


            .service('timeService', function (Restangular, localStorageService, $http) {
            })


            .service('chatService', function (Restangular, localStorageService, $http, configurationService, $rootScope, ngToast) {

                this.updateMyProfile = function (newSalesFunnel) {
                    return Restangular.one("myaccount").customPUT(newSalesFunnel);
                };

                this.addToContactList = function (a) {
                    var b = {};
                    b.userid = a;
                    return Restangular.one("contactList").customPUT(b);
                };

                this.searchUserByNaveAndData = function (newSalesFunnel) {
                    var a = {};
                    a.text = newSalesFunnel;
                    return Restangular.one("searchUser").customPOST(a);
                };


                this.rejectAuthorisation = function (userId) {
                    return Restangular.one("rejectauth/+"+userId).customPOST({});
                };


                 this.uploadAvatar = function (avatarBase64) {
                     var a = {};
                     a.data = avatarBase64;
                     return Restangular.one("uploadAvatar").customPOST(a);
                 };

                this.uploadFile = function (avatarBase64) {
                    var a = {};
                    a.data = avatarBase64.data;
                    a.filename = avatarBase64.filename;
                    return Restangular.one("uploadFile").customPOST(a);
                };




                 // for offline
                this.getUserById = function (userId) {
                    return Restangular.one("user/" + userId + "");
                };


                this.getMessageById = function (id) {
                    return Restangular.one("messagesById/" + id +"");
                };

                // for offline
                this.getAccountListFull = function () {
                    return Restangular.one("contactListWithDetails");
                };


                // for offline
                this.getNotifications = function () {
                    return Restangular.one("notifications");
                };


                // for offline
                this.getAccountSettings = function () {
                    return Restangular.one("accountSettings");
                };


                this.updateAccountSettings = function (settings) {
                    var a = {};
                    a.data = JSON.stringify ( settings);
                    return Restangular.one("accountSettings").customPOST(a);
                };


                // for offline
                this.getRoomById = function (id) {
                    return Restangular.one("room/"+id+"").get();
                };

                this.addNewRoom = function (room,type) {
                    var a = {};
                    a.name = room;
                    a.type = type;
                    return Restangular.one("room").customPUT(a);
                };

                this.leaveRoom = function (room) {
                    return Restangular.one("room/"+room+"").customDELETE();
                };


                // for offline
                this.getAllMessagesByRoomId = function (id) {
                    return Restangular.one("messages/"+id+"").get();
                };

                 // for offline
                this.getAllMessagesByRoomIdInHistory = function (id, startId, how) {
                    return Restangular.one("messages/"+id+"/"+startId+"/"+how+"").get();
                };


                this.addNewMessage = function (roomId, messageText) {
                    var a = {};
                    a.text = messageText;
                    return Restangular.one("messages/"+roomId+"").customPUT(a);
                };

                this.deleteFromContactList = function (id) {
                    return Restangular.one("contactList/"+id+"").customDELETE();
                };

                 // for offline
                this.getAllRooms = function () {
                    return Restangular.all("room").getList();
                };


                this.deleteMemberFromChat = function (roomId, userId) {
                    var a = {};
                    a.dismissMembers = [userId];
                    return Restangular.one("room/"+roomId+"").customPOST(a);
                };

                this.addMembeToChat = function (roomId, userId) {
                    var a = {};
                    a.addMembers = [userId];
                    return Restangular.one("room/"+roomId+"").customPOST(a);
                };


                this.changeRoomName = function (roomId, newName) {
                    var a = {};
                    a.name = newName;
                    return Restangular.one("room/"+roomId+"").customPOST(a);
                };

                this.softDeleteNotificationById = function (id) {
                    return Restangular.one("notifications/"+id+"").customDELETE();

                };


                // Delete from server and delete Notification Toasts
                this.deleteNotificationById = function (id) {
                    console.log($rootScope.allNotificationToasts);
                    for (var i=0; i < $rootScope.allNotificationToasts.length; i++){
                        if ($rootScope.allNotificationToasts[i].id == id){
                            ngToast.dismiss (   $rootScope.allNotificationToasts[i].obj );
                        }
                    }
                    return Restangular.one("notifications/"+id+"").customDELETE();

                };


            })






;