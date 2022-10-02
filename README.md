nepephemeral 2 base library
===========================

nepephemeral 2 is a rewrite of nepephemeral in modern model-checked C. The base
library provides a safe interface for a metadata driven derivation of passwords
from a master passphrase and a session passphrase. This interface supports
nepephemeral 0.x legacy mode as well as a PBKDF2-based mode with custom encoding
schemes. A migration view is also provided which allows multiple modes to be
stacked. When metadata lookup fails for the latest mode, fallback modes will be
checked. In this way, a newer master passphrase and mode can be added and the
user can upgrade older passwords based on older modes as they are encountered.

This library depends on either OpenSSL or LibreSSL, and RCPR. The library
encapsulates its dependency on OpenSSL and LibreSSL, providing a generic
password generation interface.
