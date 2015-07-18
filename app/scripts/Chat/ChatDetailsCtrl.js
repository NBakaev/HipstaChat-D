'use strict';

angular.module('app.chat', ['restangular', 'ngRoute', 'LocalStorageModule']).
    config(function ($routeProvider) {
        $routeProvider.
            when('/room/details/:id', {
                controller: ChatDetailsCtrl,
                templateUrl: 'views/room/details.html'
            })

            .when('/chat/userSearch', {
            controller: 'DashboardCtrl',
            templateUrl: 'views/chat/userSearch.html'
        })

            .when('/user/details/:id', {
                controller: UserDetailsCtrl,
                templateUrl: 'views/user/details.html'
            })

        ;
    });


function UserDetailsCtrl($scope, $rootScope,  Restangular, $route, $http, localStorageService, configurationService,  $location,  logger
    , chatService, $timeout) {
    $scope.currentUserId = $route.current.params.id;
}

function ChatDetailsCtrl($scope, $rootScope,  Restangular, $route, $http, localStorageService, configurationService,  $location,  logger
                        , chatService, $timeout, ngToast , $sce, $aside, $modal
) {
    $scope.currentRoomId = $route.current.params.id;

    $scope.currentRoomMessages = [];
    $scope.postsInHistory = [];

    $scope.messengesColumn = 12;
    $scope.membersColumn = 12;

    $scope.reverseBool = true;
    $scope.enterSendBtn = true;

    $scope.updatedMessagesTimes = 0;
    $scope.messageToConversation = "";

    $scope.alreadyRequestedHistoryLock = false;
    $scope.alreadyRequestedHistoryInThePastLock = false;

    $scope.lastUpdatedHistoryTime = new Date();

    // selector for main conversation box
    $scope.messageBox = $("#messageConversationBox");

///////////////////////////////////////////////////////////////////////////
    $scope.deleteNotificationsFromThisRoom = function () {
        for (var i=0; i < $rootScope.allNotifications.length; i++){
            console.log($rootScope.allNotifications[i].type + " | " + $scope.currentRoomId);

            if ( ($rootScope.allNotifications[i].type == 0
                || $rootScope.allNotifications[i].type == 1
                || $rootScope.allNotifications[i].type == 2
                || $rootScope.allNotifications[i].type == 3
                || $rootScope.allNotifications[i].type == 4 )
                && $rootScope.allNotifications[i].details == $scope.currentRoomId ){
                console.log(1);
                chatService.deleteNotificationById ($rootScope.allNotifications[i].id)
            }
        }
    };
    $scope.deleteNotificationsFromThisRoom();
///////////////////////////////////////////////////////////////////////////


    // Возвращает новые сообщения полученные из "истории"
    $scope.appendDiffHistory =  function(){
        var bIds = {};
        $scope.currentRoomMessages.forEach(function(obj){
            bIds[obj.id] = obj;
        });

        return $scope.postsInHistory.filter(function(obj){
            return !(obj.id in bIds);
        });

    };

    // Высота кнопки "Отправить" равна высоте поле ввода текста сообщения
    jQuery("#btnSendMsg").height(  jQuery("#textareaWithMsg").height()  );


    $scope.scrollbarChanged = function () {
        if (!$rootScope.useScrollable){
            $scope.messageBox.css("max-height",  "400000000px");
        }else{
            $scope.iniConversationBoxHeight();
        }
        $rootScope.saveAccountSettings ();
    };

    $scope.iniConversationBoxHeight = function () {
        if (window.screen.height == 768 && window.screen.width == 1024){
            $scope.messageBox.css("max-height",  ( window.screen.height -  $(".top-nav").height()-30) / 1.8 +"px");
        }else{
            if (window.screen.height == 1080 && window.screen.width == 1920){
                $scope.messageBox.css("max-height",  ( window.screen.height -  $(".top-nav").height()-30) / 1.8 +"px");
            }else{
                $scope.messageBox.css("max-height",  "400px");
            }
        }
    };

 //////////////////////////////////////////////////
 //////////////////////////////////////////////////
   $scope.updateMessagesData  = function () {
        if ($scope.alreadyRequestedHistoryLock) return;
        $scope.alreadyRequestedHistoryLock = true;

        if ($scope.updatedMessagesTimes < 2){
            $scope.iniConversationBoxHeight();
        }else{
            clearInterval($scope.refreshMessages);
            $scope.refreshMessages =  setInterval($scope.updateMessagesData, 3000);
        }

        chatService.getAllMessagesByRoomId($scope.currentRoomId).then(function (e) {

            var haveNewMSG = false;

            if ( $scope.currentRoomMessages.length != e.messages.length || e.messages.length == 20){
                $scope.currentRoomMessages = $rootScope.deSeserialiseMessages (e.messages, $scope.currentRoom.type);
                haveNewMSG = true;
            }
            //$scope.currentRoomMessages = e.messages;
            if ( $scope.updatedMessagesTimes < 2){
                if (!$scope.messageBox){
                    $scope.updatedMessagesTimes --;
                }
              console.warn (   $scope.messageBox.scrollTop($scope.messageBox[0].scrollHeight) );
            }

            if ($scope.postsInHistory.length > 0 && haveNewMSG){
                $scope.currentRoomMessages = $scope.currentRoomMessages.concat (  $scope.appendDiffHistory () );
            }
            $scope.updatedMessagesTimes ++;
            $scope.alreadyRequestedHistoryLock = false;

        });
        $scope.deleteNotificationsFromThisRoom();
    };


    /// show button and div
    $scope.enableDraw = function () {
        if (!$scope.drawCanvasEnabled){
            $scope.drawCanvasEnabled = true;
        }else{
            $scope.drawCanvasEnabled = false;
        }
    };

    /// show button and div
    $scope.enableUpload = function () {
        if (!$scope.uploadEnabled){
            $scope.uploadEnabled = true;
        }else{
            $scope.uploadEnabled = false;
        }
    };

    $scope.getHistoryMessageWhenScrolling = function () {
        if ($scope.updatedMessagesTimes < 2 ) return;
        if ($scope.messageBox.scrollTop()  < 20  ){

            // refresh history only one time in 3 second (when we are in the top of the message box)
            if ( ( ( new Date() - $scope.lastUpdatedHistoryTime ) / 1000 )  > 3  ){

                if ($scope.alreadyRequestedHistoryInThePastLock) return;
                $scope.alreadyRequestedHistoryInThePastLock = true;

                $scope.prevScrollHeight = $scope.messageBox[0].scrollHeight;
                console.warn ( "Prev height: " +  $scope.prevScrollHeight  );
                $scope.getFromHistory();
            }
        }

    };
    $scope.refreshistoryMsg =  setInterval($scope.getHistoryMessageWhenScrolling, 100);



    $scope.currentParticipants = $aside({scope: $scope, html:true,placement:"right", show:false, animation:"am-slide-right",  contentTemplate: 'templates/chatMembers.html'});
    $scope.showCurrentParticipantsModal = function() {
        $scope.currentParticipants.$promise.then( $scope.currentParticipants.show);
    };


    $scope.currentRoomSettings  = $aside({scope: $scope, html:true,placement:"right", show:false, animation:"am-slide-right",  contentTemplate: 'templates/roomSettings.html'});
    $scope.showRoomSettings = function() {
        $scope.currentRoomSettings.$promise.then( $scope.currentRoomSettings.show);
    };



    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    $scope.addMessageToConversation = function () {
        var messageToSend = {};
        messageToSend.textMsg = $scope.messageToConversation;

        if ($scope.drawCanvasEnabled){
            messageToSend.canvasImg = $scope.canvasDrawing.toDataURL('png');
        }

        if ($scope.yourModel){
            // if file size less then 150 kb - encode attachment to message
            // else upload to server as file
            if ( $scope.yourModel.filesize < 150000 ){
                messageToSend.file = $scope.yourModel;
                $scope.sendMessageToServer (messageToSend);
            }else{

                (function () {
                    var objectToSend = {};
                    objectToSend.data = $scope.yourModel.base64;
                    objectToSend.filename = $scope.yourModel.filename;

                    chatService.uploadFile (objectToSend).then(function (data) {
                        messageToSend.fileUrl = "/media/" + data.url;
                        messageToSend.fileUrlName = objectToSend.filename;
                        $scope.sendMessageToServer(messageToSend);
                    });
                })();
            }
        }else{
            $scope.sendMessageToServer(messageToSend);

        }
        //console.info ( $scope.yourModel.filesize);
    };


    $scope.sendMessageToServer = function (messageToSend) {
        var secretRommKey = "3eddffewfew";

        if ($scope.currentRoom.type == 1){
            //messageToSend.hash = CryptoJS.SHA512( JSON.stringify( messageToSend )).toString();
            var encryptedPart = CryptoJS.AES.encrypt(JSON.stringify( messageToSend ), secretRommKey).toString();
            messageToSend = encryptedPart;
        }

        chatService.addNewMessage ( $scope.currentRoomId, messageToSend).then(function (e) {
            logger.logSuccess("Сообщение отправлено!");
            $scope.updateMessagesData ();
            $scope.canvasDrawing.clear();

            $scope.messageBox.scrollTop($scope.messageBox[0].scrollHeight + 9999999)

            $scope.drawCanvasEnabled = false;
            $scope.yourModel = null;
            $scope.messageToConversation = "";
            $scope.resetFile ();
        } )
    };






    $scope.alreadyInChatFilter = function(obj){
        if (!$scope.currentRoom) return true;
        for (var i=0; i <  $scope.currentRoom.members.length ;i++){
            if (obj.id ==  $scope.currentRoom.members[i]) return false;
        }
        return true;

    };

    $scope.resetFile = function () {
        var e = $("#uploadFile");
        e.wrap('<form>').closest('form').get(0).reset();
        e.unwrap();
    };



    $scope.base64toBlob = function (b64Data, contentType, sliceSize) {
        contentType = contentType || '';
        sliceSize = sliceSize || 512;

        var byteCharacters = atob(b64Data);
        var byteArrays = [];

        for (var offset = 0; offset < byteCharacters.length; offset += sliceSize) {
            var slice = byteCharacters.slice(offset, offset + sliceSize);

            var byteNumbers = new Array(slice.length);
            for (var i = 0; i < slice.length; i++) {
                byteNumbers[i] = slice.charCodeAt(i);
            }

            var byteArray = new Uint8Array(byteNumbers);

            byteArrays.push(byteArray);
        }

        var blob = new Blob(byteArrays, {type: contentType});
        return blob;
    };


    $scope.downloadBase64 = function (file) {
        var blob = $scope.base64toBlob(file.base64, file.filetype);
        saveAs(blob, file.filename);
    };


    // refresh only 2 times to get right height of display. then will be another speed of updates (3000)
   $scope.refreshMessages =  setInterval($scope.updateMessagesData, 100);

    //// Убираем обновления после ухода со страницы
    $rootScope.$on("$locationChangeStart", function () {
        console.info("Уход из комнаты: " + $scope.currentRoom.name + " с ID: " + $scope.currentRoom.id);
        clearInterval($scope.refreshMessages);
        clearInterval($scope.refreshistoryMsg);
        clearInterval($scope.updatingCanvas);
        $scope.currentRoomSettings.hide();
    });


    $scope.getFromHistory = function () {
        if (!$scope.currentRoomMessages[ $scope.currentRoomMessages.length-1]) return;
        var elel = $scope.currentRoomMessages[ $scope.currentRoomMessages.length-1].id;

        chatService.getAllMessagesByRoomIdInHistory($scope.currentRoomId, elel, 20).then(function(e){
            $scope.currentRoomMessages =  $scope.currentRoomMessages.concat(  $rootScope.deSeserialiseMessages (e.messages, $scope.currentRoom.type )   );

            $scope.postsInHistory = $scope.postsInHistory.concat (e.messages);

            console.log(e.messages.length);
            console.info ( "Текщая высотаЖ " +  $scope.messageBox[0].scrollHeight);
            console.info ( "Предыдущая высотаЖ " +  $scope.prevScrollHeight);

            if (e.messages.length > 0){
                $scope.messageBox.scrollTop($scope.prevScrollHeight) ;
            }

            $scope.lastUpdatedHistoryTime = new Date();
            $scope.alreadyRequestedHistoryInThePastLock = false;
        })
    };

    $scope.addMemberToChat = function (userId) {
        chatService.addMembeToChat($scope.currentRoomId, userId).then(function (e) {
            logger.logSuccess("Пользователь добавлен в чат!");
            $scope.updateRoom();
        } )
    };

    $scope.deleteMemberFromChat = function (userId) {
        chatService.deleteMemberFromChat($scope.currentRoomId, userId).then(function (e) {
            logger.logSuccess("Пользователь удален из чата!");
            $scope.updateRoom();
        } )
    };

    $scope.changeRoomName = function () {
        var userId = $scope.newRoomName;
        console.info($scope.newRoomName);
        chatService.changeRoomName($scope.currentRoomId, userId).then(function (e) {
            logger.logSuccess("Название изменено на: " + userId);
        } )
    };

    $scope.updateRoom = function(){
        chatService.getRoomById ($scope.currentRoomId).then(function (e) {
            $scope.newRoomName = e.name;
            $scope.currentRoom = e;
        });
    };
    $scope.updateRoom();


    $scope.updateCanvasView = function () {
        $scope.canvasDrawing = window.canvas;

        var A = new SimplBox(document.querySelectorAll(".imgToZoom"), {
            quitOnDocumentClick: true
        });
        A.init();

    };

    $scope.updatingCanvas = setInterval ($scope.updateCanvasView , 1000);




};