//
//  TritonPlayer.h
//  TritonPlayer
//
//  Copyright 2014 Triton Digital. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
#import <AudioToolbox/AudioQueue.h>
#import <CoreMedia/CMTime.h>
#import <AVFoundation/AVFoundation.h>

// SDK Version
extern NSString *const TritonSDKVersion;
// Settings dictionary keys

extern NSString *const SettingsEnableLocationTrackingKey;
extern NSString *const SettingsStationNameKey;
extern NSString *const SettingsMountKey; /// The mount for live content
extern NSString *const SettingsContentURLKey; /// The url for an on-demand content
extern NSString *const SettingsContentTypeKey; /// The type of on-demand content. If not specified, the app will try to guess it based on the url, otherwise it will use the "Other" profile. The value must be one of SettingsContentType enum values.
extern NSString *const SettingsAppNameKey;
extern NSString *const SettingsBroadcasterKey;
extern NSString *const SettingsStreamParamsExtraKey; /// A NSDictionary containing parameters for targeting, player capabilities and other information passed to the server. The possible keys are listed below.
extern NSString *const SettingsTtagKey;  // An Array containing custom TTags
extern NSString *const SettingsLowDelayKey;  //-1 (AUTO), 0 (DISABLED), 2 … 60 for seconds 
extern NSString *const SettingsPlayerServicesRegion;  // To use a specific PlayerServices depending on the region, e.g: EU,AP
extern NSString *const SettingsBitrateKey; //Set the bitrate for low delay
extern NSString *const SettingsDistributionParameterKey; //Set the distributor targeting value
/// Extra parameters for location targeting

extern NSString *const StreamParamExtraLatitudeKey; // Floating-point value: -90.0 to 90.0
extern NSString *const StreamParamExtraLongitudeKey; // Floating-point value: -180.0 to 180.0
extern NSString *const StreamParamExtraPostalCodeKey; // Valid postal or ZIP code, without spaces. E.g., 89040 or H3G1R8.
extern NSString *const StreamParamExtraCountryKey; // ISO 3166-1 alpha-2 two-letter country code (e.g., US)

/// Extra parameters for demographic targeting
extern NSString *const StreamParamExtraAgeKey; // Integer value: 1 to 125
extern NSString *const StreamParamExtraDateOfBirthKey; // String formatted as YYYY-MM-DD
extern NSString *const StreamParamExtraYearOfBirthKey; // Integer value: 1900 to 2005
extern NSString *const StreamParamExtraGenderKey; // “m” or “f” (case-sensitive)

extern NSString *const SettingsDebouncingKey; // Play debouncing in seconds:  float value e.g: 0.2

/// Custom segment id targeting
extern NSString *const StreamParamExtraCustomSegmentIdKey; // Integer value: 1 to 1000000

/// Banner capabilities
extern NSString *const StreamParamExtraBannersKey; // See Streaming guide 5.3.1 for supported banners

/// Token authorization
extern NSString *const StreamParamExtraAuthorizationTokenKey; // An string with a JST token.



/// Represents the types of on-demand content supported by the player
typedef NS_ENUM(NSInteger, PlayerContentType) {

    /// The player will use the FLV decoder to play and receive cue points.
    PlayerContentTypeFLV,
    
    /// The player will use HLS with side-band metadata.
    PlayerContentTypeHLS,
    
    /// The player will use the default profile.
    PlayerContentTypeOther
};

extern NSString *const TritonPlayerDomain;

/// Errors generated by TritonPlayer
typedef NS_ENUM(NSInteger, TDPlayerError) {
    
    /// The specified mount doesn’t exist
    TDPlayerMountNotFoundError = 3000,
    
    /// The mount is geoblocked
    TDPlayerMountGeoblockedError = 3001,
    
    /// A required parameter is missing or an invalid parameter was sent
    TDPlayerMountBadRequestError = 3002,
    
    /// The version of the provisioning doesn't exist
    TDPlayerMountNotImplemntedError = 3003,
    
    /// The host doesn't exist
    TDPlayerHostNotFoundError = 3004,
    
    /// The device is muted
    TDPlayerDeviceMuted = 3005    
    
};

/// States representing Triton Player playback life-cycle
typedef NS_ENUM(NSInteger, TDPlayerState) {
    
    /// Player is stopped. The playhead position returns to the beginning.
    kTDPlayerStateStopped,
    
    /// Player is currently playing
    kTDPlayerStatePlaying,
    
    /// Player is connecting to the stream
    kTDPlayerStateConnecting,
    
    /// Player is paused. The playhead position is kept at the last position. Not available for live streams.
    kTDPlayerStatePaused,
    
    /// Player is on error state
    kTDPlayerStateError,
    
    /// The end of the media has been reached. Not available for live streams.
    kTDPlayerStateCompleted
};

typedef NS_ENUM(NSInteger, TDPlayerInfo) {
    /// The player established connection with the stream. It will soon start playing.
    kTDPlayerInfoConnectedToStream,
    
    /// The stream is buffering. The buffer percentage can be queried by the key InfoBufferingPercentageKey in the extra dictionary for player:didReceiveInfo:andExtra:
    kTDPlayerInfoBuffering,
    
    /// The provided mount is geoblocked. The player was redirected to an alternate mount. It's mount name can be obtained by the key InfoAlternateMountNameKey in the extra dictionary.
    kTDPlayerInfoForwardedToAlternateMount
};

extern NSString *const InfoBufferingPercentageKey;
extern NSString *const InfoAlternateMountNameKey;

@class CuePointEvent;
@class TritonPlayer;

/**
 * TritonPlayerDelegate defines methods you can implement to handle streaming state notifications and to receive cue point events.
 */
@protocol TritonPlayerDelegate<NSObject>

@required

/// @name Handling state changes

/**
 * Called when the player's state changed.
 *
 * @param player The player whose state changed
 * @param state The new state of the player
 */

- (void) player:(TritonPlayer *) player didChangeState:(TDPlayerState) state;

@optional

/// @name Receiving player info

/**
 * Called when the player's state changed.
 *
 * @param player The player whose state changed
 * @param state The new state of the player
 */

- (void) player:(TritonPlayer *) player didReceiveInfo:(TDPlayerInfo) info andExtra:(NSDictionary *) extra;

/// @name Handling cue point events

/**
 * Called when there's a Cue Point available to be processed. A NSDictionary is passed containing the Cue Point metadata. All the available keys are defined in CuePointEvent.h.
 * See STWCue_Metadata_Dictionary.pdf for more details on the available cue point information.
 *
 * @param player The player which is receiving cue point events
 * @param cuePointEvent A CuePointEvent object containing all cue point information.
 */

- (void)player:(TritonPlayer *) player didReceiveCuePointEvent:(CuePointEvent *)cuePointEvent;
- (void)player:(TritonPlayer *) player didReceiveAnalyticsEvent:(AVPlayerItemAccessLogEvent *)analyticsEvent;


/// @name Handling interruptions

/**
 * Notifies that an audio interruption is about to start (alarm, phone call, etc.). The application has the opportunity to take the proper actions: stop the player, lower the volume, etc.
 *
 * @param player The TritonPlayer object which is being interrupted.
 */

- (void)playerBeginInterruption:(TritonPlayer *) player;

/**
 * Notifies about a finished interruption. It's the proper moment to resume the player, raise the volume, etc.
 *
 * @param player The TritonPlayer object whose interruption is ending.
 */

- (void)playerEndInterruption:(TritonPlayer *) player;



-(void)player:(TritonPlayer *)player didReceiveMetaData: (NSDictionary *)metaData;

@end

/** 
 * TritonPlayer handles the playback of stations provided by Triton Digital. It also supports receiving CuePoint events with metadata for track information, ads etc.
 */

@interface TritonPlayer : NSObject

/// @name Querying player information

/**
 * The current state of the player
 */

@property (readonly) TDPlayerState state;

/**
 * Returns the current playback duration in seconds. Not available for live streams.
 */

@property (readonly) NSTimeInterval playbackDuration;

/**
 * Returns the current playback position in seconds.
 */

@property (readonly) NSTimeInterval currentPlaybackTime;

/**
 * Tells whether the player is streaming audio.
 */

@property (readonly) BOOL isExecuting;

/**
 * When the player's state is kTDPlayerStateError, this property contains the associated error. It's nil for all other states.
 */

@property (readonly) NSError *error;

/**
 * The underlying audio queue which playbacks the audio. Use this when you need to process or analyze the audio data. Ex. When building a spectrum analyzer.
 *
 * @returns The underlying AudioQueue
 */

- (AudioQueueRef)getAudioQueue;

/**
 * Returns the current library version
 *
 * @return A string containing the current library version.
 */

- (NSString *)getLibVersion;

/**
 * Informs if the network is available.
 *
 * @return Whether the network is available.
 */

- (BOOL)isNetworkReachable;


/// @name Location targeting

/**
 * The most recent user location available for audience targeting.
 */

@property (readonly) CLLocation *targetingLocation;

/// @name Handling interruptions

/**
 * When an interruption ends (phone call, alarm, siri etc.) this flags will be true when it is appropriate to resume playback without waiting for user input.
 *
 * If the user ignored a call, it means that he/she wants to continue listening to the app and the flag will be YES. On the other side, if the interruption was caused by the Music app or other audio app being executed,
 * it means that he/should explicitly play again to continue listening to the stream and the flag will retur NO.
 */

@property (readonly) BOOL shouldResumePlaybackAfterInterruption;

/// @name Creating a TritonPlayer

/** 
 * Instantiate a new player using the specified settings
 *
 * @param inDelegate The delegate for handling stream callbacks and CuePoint events
 * @param settings A NSDictionary containing station parameters.
 */

- (id)initWithDelegate:(id<TritonPlayerDelegate>)inDelegate andSettings:(NSDictionary *) settings;

/// @name Updating player settings

/** 
 * Update player settings. All the information passed overrides the current settings and will take effect the next time the play method is called.
 *
 * When changing to a new station, this method must be called before calling play on the new station.
 *
 * @param settings A NSDictionary containing station parameters.
 */

- (void)updateSettings:(NSDictionary *) settings;

/// @name Reproduction flow

/** 
 * Plays the current stream with the configuration from the settings dictionary.
 *
 * @see updateSettings:
 */

- (void)play;

/** 
 * Stops the current stream
 */

- (void)stop;

/**
 * Pauses the current stream. In case of a live stream, it's the same as calling stop.
 */

- (void)pause;

/**
 * Moves the cursor to the specified time interval. Only works if the media is seekable.
 *
 * @param interval A time interval representing the time elapsed since the start of the audio
 */

- (void)seekToTimeInterval:(NSTimeInterval) interval;

/**
 * Moves the cursor to the specified time and invokes the block specified when the seek operation has either been completed or been interrupted. Only works if the media is seekable.
 *
 * @param time A CMTime representing the time elapsed since the start of the audio
 * @param completionHandler The block to be executed when the seek operation has been completed or interrupted.
 */

- (void)seekToTime:(CMTime)time completionHandler:(void (^)(BOOL finished))completionHandler;

/// @name Controlling the volume

/**
 * Mute current playing audio
 */

- (void)mute;

/**
 * Unumte current playing audio
 */

- (void)unmute;

/**
 * Set volume of current playing audio
 *
 * @param volume a float between 0-1.
 */

- (void)setVolume:(float)volume;

/**
 * Set external playback on underlying AVPlayer
 */

- (void)setAllowsExternalPlayback:(BOOL)allow;

/**
 *  Return the stream URL for Cast Devices
 *
 */
- (NSString*) getCastStreamingUrl;

/**
 *  Return  the Side Band Metadata URL
 *
 */
-(NSString*) getSideBandMetadataUrl;


/**
 *  Format Player state into String
 *
 * @param player state.
 */
+(NSString*) toStringState:(TDPlayerState)state;


@end
