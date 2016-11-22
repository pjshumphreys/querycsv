/*
Picup helper
================

This document describes version 2 of Picup. For earlier versions, visit http://localhost/picupapp/v1/scratchpad.html

About:
----------------
Picup App allows webapps to request files from an iOS device in place of the traditional 
file-input form field. Call Picup2.convertFileInput for 
each file input field that allows iOS file uploads. This function should be called 
after the document has loaded, such as in an onLoad observer.

NOTE: Make sure that the client is iOS before you replace the file inputs.

Examples:
----------------
• Converting a file-upload field into a Picup button:

    Picup2.convertFileInput( fileInputField,  { 'referrername' : encodeURI('My Web App'),
                                             	'purpose'      : encodeURI('Upload A Photo') });

• Opening an existing upload in the Picup app:

    Picup2.openFileWithId('p12');                                            

• Generating a Picup URL:

    // URL to upload a photo:
    
    Picup2.urlForOptions('new', { 'referrername' : encodeURI('My Web App'),
                                  'purpose'      : encodeURI('Upload A Photo') });
                                  
    // URL to view a photo:
    Picup2.urlForOptions('view', { 'picID' : 'p12' });            
    
• Defining a custom callback handler:

    // To handle the return data from the Picup app, define the Picup2.callbackHandler
    // function, which is passed a query string of return values.
    
	// If Picup2.callbackHandler is defined, the page will observe the location.hash.
	// If it changes (e.g. the callback URL returns control to the page that launched Picup), 
	// Picup2.callbackHandler will be called with an object containing key/value pairs of the 
	// parameters.
	
	// Simple example:
	
    Picup2.callbackHandler = function(data){
		for(var key in data){
			console.log(key + " = " + data[key]);
		}
    }

Additional Notes:
----------------
If you have multiple file input fields that use Picup, the last input field that launched 
the app will be referenced by Picup2.activeFileInput. 

Mobile Browser Support:
----------------
If the user visits your site from Chrome or Opera, the callback URL will automatically be modified
to use the custom URL scheme of their browser so that Picup returns them to the calling app.
If you wish to disable this feature, set Picup2.shouldConvertCallbackURLScheme = false;

More info:
----------------
API documentation, with explainations of the possible options can be found at http://picupapp.com

================
© William Lindmeier, 2012
http://picupapp.com

*/

var Picup2 = {
    
    customURLScheme : 'fileupload2://',     
    windowname : 'fileupload',
    activeFileInput : null,
    currentHash : '', 
    hashObserverId : null,
    appStoreURL : "itms-apps://itunes.com/apps/Picup",
    downloadPrompt : "This form requires the Picup app. Would you like to install it now?",
    shouldConvertCallbackURLScheme : true,
    version : "2.1.2",

    // Thanks to Devin Stetler for contributing this function.
    // NOTE: This approach works in Mobile Safari but not Chrome
    confirmAppLaunched : function(){
        var clickedAt = +new Date;
        setTimeout(function(){
           if (+new Date - clickedAt < 2000){
               // If it's w/in 2 seconds we'll assume the app wasn't launched               
               clearInterval(Picup2.hashObserverId);
               Picup2.appNotInstalledHandler();
           }
        }, 500);   
    },
    
    // The default handler opens the Picup page in the AppStore
    // if the app is not installed.
    // This can be overridden to execute customized behavior (e.g. a UI change or alert).
    // NOTE: This only works in Mobile Safari at the moment.
    appNotInstalledHandler : function(){  
        isMobileSafari = MobileBrowserName() == 'Safari';
        if(isMobileSafari && confirm(Picup2.downloadPrompt)){             
            window.location.href = Picup2.appStoreURL;
        }
    },
    
	// Override this as a function to handle hash changes
    callbackHandler : null,

    openFileWithId : function(picID){
        window.location.href = Picup2.urlForOptions('view', {'picID' : picID});
        return false;
    },
    
    convertFileInput : function(inputOrInputId, options){

        var input = inputOrInputId;
        if(typeof(inputOrInputId) == 'string'){
            input = document.getElementById(inputOrInputId);
        }

        if (window.navigator.standalone) {
            
            // This is a Springboard link.
            // The button approach doesn't work because standalone pages dont
            // allow window.location.href = ...
            // We're not actually losing any functionality because apps can't
            // return control to standalone webapps.
            
            var inputId = input.id;
            if(!inputId){
                // Give it one so we can track it
                inputId = "picupInput_"+Math.random().toString();
                input.id = inputId;
            }
            var linkClass = "picup_input_link";
            
            // Remove any existing link for this input
            var inputLinks = document.getElementsByClassName(linkClass);
            for(var i=0;i<inputLinks.length;i++){
                var linkEl = inputLinks[i];
                if(linkEl.rel == inputId){
                    linkEl.parentNode.removeChild(linkEl);
                }
            }
            
            var inputLink = document.createElement('a');
            inputLink.className = linkClass;
            inputLink.rel = inputId;
            inputLink.innerHTML = "Choose File...";
            inputLink.href = Picup2.urlForOptions('new', options);
            input.parentNode.insertBefore(inputLink, input);
            // Hide the real input
            input.style.visibility = "hidden";
            
        } else {

            // If the page was opened in a browser, just convert the input.
            input.type = 'button';
            input.value = "Choose File...";
            input.picupOptions = options;
            input.onclick = function(){     	    

                Picup2.activeFileInput = this;

                // start the observer
                if(Picup2.callbackHandler){
                    Picup2.currentHash = window.location.hash;
                    Picup2.hashObserverId = setInterval('Picup2.checkHash()', 500);
                }			

                window.location.href = Picup2.urlForOptions('new', this.picupOptions);

                // Picup2.confirmAppLaunched();

            };
            input.disabled = false;

        }

        return false;
    },

    checkHash : function(){	    
    	if(window.location.hash != Picup2.currentHash){
    		// The hash has changed
    		clearInterval(Picup2.hashObserverId);
    		Picup2.currentHash = window.location.hash;
    		
	    	var hash = window.location.hash.replace(/^\#/, '');
    		var paramKVs = hash.split('&');
			var paramHash = {};
			for(var p=0;p<paramKVs.length;p++){
				var kvp = paramKVs[p];
				// we only want to split on the first =, since data:URLs can have = in them
				var kv = kvp.replace('=', '&').split('&');
				paramHash[kv[0]] = kv[1];
			}
			Picup2.callbackHandler(paramHash);
    	}
    },
    
    urlForOptions : function(action, options){
	    var url = Picup2.customURLScheme+action+'?';
    	var params = [];
    	for(var param in options){
    	    optionValue = options[param];
    	    paramLowerCase = param.toLowerCase();
    	    if((paramLowerCase == 'callbackurl' || paramLowerCase == 'cancelurl') && 
    	       Picup2.shouldConvertCallbackURLScheme){
    	           browserName = MobileBrowserName();
    	           if(browserName == 'Opera'){
    	               optionValue = optionValue.replace(/^http/, 'ohttp');
    	           }else if(browserName == 'Chrome'){
    	               optionValue = optionValue.replace(/^http/, 'googlechrome');
	               }
    	    }
    		params.push(param+'='+optionValue);
    	}
    	var uploadURL = url + params.join('&');
    	return uploadURL;
    },
    
    isMobileIOS : function(){        
        var agent = navigator.userAgent.toLowerCase();
        return (agent.indexOf('iphone') != -1) || (agent.indexOf('ipad') != -1);        
    }    
    
};

function MobileBrowserName()
{
    var agent = navigator.userAgent.toLowerCase();
    if(agent.indexOf('opera') != -1){
        return 'Opera';
    }else if(agent.indexOf('crios') != -1){
        return 'Chrome';
    }
    return 'Safari';
}
