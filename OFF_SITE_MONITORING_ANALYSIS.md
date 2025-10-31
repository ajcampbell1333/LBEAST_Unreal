# Off-Site Monitoring Analysis

## Current Implementation Status for Off-Site Use

### ✅ What WILL Work Over Internet

**Command Protocol (UDP Direct Connection)**
- **Status:** ✅ **CAN work** over internet with proper network configuration
- **Port:** 7779 (UDP)
- **Method:** Direct UDP connection (not broadcast)
- **Requirements:**
  - Server must have public IP or port-forwarded private IP
  - Firewall must allow UDP port 7779 inbound
  - Command Console must know server's IP address (manual entry)
  - Network path must allow UDP packets

**How it works:**
- Command Console creates UDP socket and sends packets directly to server IP:port
- Server listens on UDP port 7779 and receives commands
- Works over internet same as LAN, just requires proper routing

### ❌ What WON'T Work Over Internet

**Server Beacon (UDP Broadcast Discovery)**
- **Status:** ❌ **LAN-ONLY** - Does NOT work over internet
- **Port:** 7778 (UDP broadcast)
- **Method:** UDP broadcast (`SetBroadcastAddress()`)
- **Why it fails:** UDP broadcasts are limited to local network segment. Routers don't forward broadcasts across internet.
- **Impact:** No automatic server discovery over internet. Must manually enter server IP.

**Current Implementation:**
```cpp
// LBEASTServerBeacon.cpp line 242
BroadcastAddr->SetBroadcastAddress();  // This is LAN-only!
```

## Off-Site Monitoring Scenario Analysis

### Scenario: Command Console on GCP VM, Server at LBE Location

**What Works:**
1. ✅ **Command Protocol** - CAN connect if:
   - LBE server has public IP or port-forwarded IP
   - UDP port 7779 is open on firewall
   - IP address is manually entered in Command Console
   - No NAT traversal issues (or NAT is properly configured)

**What Doesn't Work:**
1. ❌ **Auto-Discovery** - Server Beacon won't find servers across internet
   - Solution: Manually enter server IP in Remote Server IP field
   - Server Beacon still won't find it, but Command Protocol can connect directly

2. ⚠️ **Status Monitoring** - Server Beacon status updates won't work over internet
   - Currently relies on Beacon for real-time status (player count, state, etc.)
   - Command Protocol can send commands but status updates come via Beacon
   - **Impact:** May need to add status requests via Command Protocol

## Current Limitations

### 1. No Internet Auto-Discovery
- Server Beacon uses UDP broadcast (LAN-only)
- Must manually enter server IP for remote connections
- Could be solved with alternative discovery (DNS, central registry, etc.)

### 2. Status Updates Depend on Beacon
- Real-time status (player count, experience state) comes via Server Beacon
- Beacon is LAN-only
- **Workaround:** Could add `RequestStatus` command via Command Protocol (already exists but may not be fully implemented)

### 3. No Authentication/Security
- Command Protocol has no authentication
- Anyone with server IP and port can send commands
- **Security Risk:** Exposing command port to internet without auth is dangerous

### 4. NAT Traversal
- UDP can work through NAT, but depends on NAT type
- May require port forwarding or UPnP
- No STUN/TURN support for complex NAT scenarios

## What Would Need to Be Added

### For Full Off-Site Monitoring Support:

1. **Status via Command Protocol** (Medium Priority)
   - Implement status request/response in Command Protocol
   - Allow Command Console to query server status directly
   - Doesn't require Beacon

2. **Authentication** (High Priority - Security)
   - Add authentication to Command Protocol
   - Shared secret or token-based auth
   - Prevent unauthorized access

3. **Internet Discovery Alternative** (Low Priority)
   - DNS-based discovery
   - Central registry server
   - Manual IP entry (simplest solution)

4. **Encryption** (Medium Priority - Security)
   - Encrypt command protocol traffic
   - Prevent command interception/man-in-the-middle

## Recommendation for Current Implementation

**Can it work off-site?** **PARTIALLY**

✅ **Works:**
- Command Protocol CAN connect over internet (if network configured)
- Can send Start/Stop commands
- Can send other control commands

❌ **Doesn't Work:**
- Auto-discovery (must manual IP entry)
- Real-time status updates via Beacon (may need status polling via commands)

⚠️ **Security Concerns:**
- No authentication - exposing command port to internet is risky
- No encryption - commands sent in plain text
- Recommend VPN or firewall restrictions for production use

**For Production Off-Site Monitoring:**
1. Use VPN between GCP VM and LBE location
2. Manually configure server IP (no auto-discovery)
3. Implement authentication (high priority)
4. Add status polling via Command Protocol (if real-time status needed)

## Quick Fix for Status Updates

The `RequestStatus` command already exists in the protocol. To enable status over internet:
1. Server implements status response in `OnCommandReceived()`
2. Command Console periodically sends `RequestStatus` command
3. Server responds with current status (player count, state, etc.)
4. Works over internet via Command Protocol (no Beacon needed)

