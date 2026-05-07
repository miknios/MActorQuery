# MActorQuery

A tag-based actor query system for Unreal Engine. MActorQuery enables developers to efficiently discover, manage, and interact with actors in the world using a registry-based query system powered by Gameplay Tags.

This plugin is based on a pattern from the Flow Graph plugin, adapting its approach to tagging and querying actors for flexible actor discovery in gameplay systems.

## Overview

MActorQuery provides a centralized way to register and query actors at runtime using Gameplay Tags, eliminating the need for hard references or scattered actor lookups. This is particularly useful for:

- **Dynamic actor relationships**: Reference dynamically spawned actors without hardcoding dependencies
- **Gameplay mechanics**: Finding specific actors based on their role (e.g., "Interactable.Door", "NPC.Vendor")
- **Reduced coupling**: Decouple gameplay systems from explicit actor references
- **Editor tools**: Locating tagged actors in the editor for tool development

## Key Features

- **Tag-Based Discovery**: Register actors with Gameplay Tags and query them by tags
- **Flexible Queries**: Search by single tag, multiple tags with Any/All matching
- **Hierarchical Tag Support**: Optional searching through child tags for filtering
- **Class Filtering**: Further refine queries by actor class
- **Blueprint Compatible**: Full Blueprintable API for designers and scripters
- **Delegate Support**: Subscribe to actor registration/unregistration events
- **Dynamic Spawning**: Seamlessly track dynamically spawned and destroyed actors
- **Editor Support**: Query actors in the editor world for development tools
- **Runtime & Editor**: Works in both gameplay and editor contexts

## Architecture & Components

### 1. MActorQueryComponent

The core actor-associated component that marks an actor as queryable.

**Key Responsibilities**:
- Stores a collection of gameplay tags identifying the actor
- Automatically registers/unregisters with the subsystem during lifecycle
- Provides tag management functions (add, remove, set)

**Usage**:
Simply add a `UMActorQueryComponent` to any actor you want to make queryable. The component handles automatic registration during `BeginPlay()` and cleanup during `EndPlay()`.

**Key Methods**:
```cpp
// Get tags
UFUNCTION(BlueprintCallable)
FGameplayTag GetPrimaryTag() const;

UFUNCTION(BlueprintCallable)
FGameplayTagContainer GetTagContainer() const;

// Tag management
UFUNCTION(BlueprintCallable)
void SetPrimaryTag(const FGameplayTag& PrimaryTag);

UFUNCTION(BlueprintCallable)
void SetTagContainer(const FGameplayTagContainer& InTagContainer);

UFUNCTION(BlueprintCallable)
void AddTag(const FGameplayTag& TagToAdd);

UFUNCTION(BlueprintCallable)
void RemoveTag(const FGameplayTag& TagToRemove);

// Manual registration control
UFUNCTION(BlueprintCallable)
void RegisterQueryComponent();

UFUNCTION(BlueprintCallable)
void UnregisterQueryComponent();
```

### 2. MActorQuerySubsystem

A World Subsystem that maintains a registry of all queryable actors and handles query operations.

**Key Responsibilities**:
- Maintains a registry of tags to query components, handling all dynamically spawned/destroyed actors
- Executes actor queries based on search criteria
- Manages component registration/unregistration as actors spawn and despawn
- Broadcasts events when actors are registered/unregistered

**Accessing the Subsystem**:
```cpp
// In C++
UMActorQuerySubsystem* QuerySubsystem = UMActorQuerySubsystem::Get(GetWorld());

// In Blueprint
Get Actor Query Subsystem (node)
```

**Key Methods**:
```cpp
// Query multiple actors
UFUNCTION(BlueprintCallable)
void QueryActors(const FMActorQueryInput& QueryInput, TArray<AActor*>& OutActors) const;

// Query single actor
UFUNCTION(BlueprintCallable)
AActor* QueryActor(const FMActorQueryInput& QueryInput) const;

// Template helper for typed queries
template <typename T>
T* GetActorOfClass(FGameplayTag _ActorTag);

// Event delegates
UPROPERTY(BlueprintAssignable)
FMOnQueryableActorRegisteredSignature OnQueryableActorRegisteredDelegate;

UPROPERTY(BlueprintAssignable)
FMOnQueryableActorRegisteredSignature OnQueryableActorUnregisteredDelegate;
```

### 3. Query System - FMActorQueryInput

The query configuration structure that defines what actors to search for.

**Location**: `MActorQueryTypes.h`

**Features**:
- Single tag queries or multiple tag queries
- Any/All matching modes for multiple tags
- Optional hierarchical tag matching (includes child tags)
- Optional actor class filtering

**Structure**:
```cpp
struct FMActorQueryInput
{
    // Single tag mode vs multiple tags (TagContainer) mode
    bool bUseTagContainer = false;
    
    FGameplayTag QueryTag;                      // Used when !bUseTagContainer
    FGameplayTagContainer QueryTagContainer;    // Used when bUseTagContainer
    
    // For tag container: Any = match any tag, All = match all tags
    EMQueryMatchType QueryMatchType = EMQueryMatchType::Any;
    
    // Include tags and their children in search (hierarchical matching)
    bool bIncludeChildrenTags = false;
    
    // Filter results by actor class
    bool bMatchActorClass = false;
    TSubclassOf<AActor> ActorClassToMatch;
};
```

**Helper Methods**:
```cpp
// Create a query for single actor with tag
static FMActorQueryInput GetActorForTag(FGameplayTag Tag);

// Create a query for multiple actors with tag
static FMActorQueryInput GetActorsForTag(FGameplayTag Tag);

// Check if query configuration is valid
bool IsValid() const;

// Get tags that would match this query
FGameplayTagContainer GetMatchedTags() const;

// String representation for debugging
FString ToString() const;
```

### 4. MActorQueryStatics

Blueprint Function Library providing convenient access to query creation helpers.

**Available Functions**:
```cpp
UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Actor Query Input (Actor For Tag)")
static FMActorQueryInput GetActorQueryInput_ActorForTag(FGameplayTag Tag);

UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Actor Query Input (Actors For Tag)")
static FMActorQueryInput GetActorQueryInput_ActorsForTag(FGameplayTag Tag);
```

## Usage Examples

### Basic Setup

1. **Create Gameplay Tags**
   - In your project's `DefaultGameplayTags.ini` or through the UE Gameplay Tags editor:
   ```
   +GameplayTagList=(Tag="NPC.Vendor",DevComment="Marks NPCs that are vendors")
   +GameplayTagList=(Tag="NPC.Enemy",DevComment="Marks hostile NPCs")
   +GameplayTagList=(Tag="GameManager.Audio",DevComment="Main audio manager actor")
   +GameplayTagList=(Tag="Interactable.Door",DevComment="Interactive doors")
   ```

2. **Add Component to Actor**
   - In your actor (C++ or Blueprint):
   ```cpp
   // C++: In constructor or Detail panel
   MyQueryComponent = CreateDefaultSubobject<UMActorQueryComponent>("QueryComponent");
   ```
   - Or in Blueprint: Add `MActorQueryComponent` in Details panel

3. **Assign Tags**
   - In the Details panel, set the TagContainer property of the `MActorQueryComponent`

### Example 1: Find a Specific NPC in C++

```cpp
void APlayerCharacter::FindVendor()
{
    // Get the subsystem
    UMActorQuerySubsystem* QuerySubsystem = UMActorQuerySubsystem::Get(GetWorld());
    
    // Create a query for a vendor NPC
    FMActorQueryInput Query = FMActorQueryInput::GetActorForTag(
        FGameplayTag::RequestGameplayTag(FName("NPC.Vendor"))
    );
    
    // Execute query
    AVendorNPC* Vendor = Cast<AVendorNPC>(
        QuerySubsystem->QueryActor(Query)
    );
    
    if (Vendor)
    {
        Vendor->OpenShop(this);
    }
}
```

### Example 2: Find All Vendors Using Tag Container (Any Match)

```cpp
void APlayerCharacter::FindNearbyVendors()
{
    UMActorQuerySubsystem* QuerySubsystem = UMActorQuerySubsystem::Get(GetWorld());
    
    // Create a query with multiple tag options
    FMActorQueryInput Query;
    Query.bUseTagContainer = true;
    Query.QueryTagContainer.AddTag(
        FGameplayTag::RequestGameplayTag(FName("NPC.Vendor"))
    );
    Query.QueryMatchType = EMQueryMatchType::Any;
    
    TArray<AActor*> Vendors;
    QuerySubsystem->QueryActors(Query, Vendors);
    
    for (AActor* Vendor : Vendors)
    {
        if (ABNPCCharacter* NPCVendor = Cast<ABNPCCharacter>(Vendor))
        {
            NPCVendor->OpenShop(this);
        }
    }
}
```

### Example 3: Find Door by Class and Tag (Blueprint)

**In Blueprint**:
1. Create a query input using "Get Actor Query Input (Actor For Tag)" with tag "Interactable.Door"
2. Pass it to "Query Actors" on the Actor Query Subsystem
3. Filter results by casting to your door class
4. Call door interaction

### Example 4: Hierarchical Tag Queries

If you have a tag hierarchy like:
```
Interactable
├─ Door
├─ Chest
└─ NPC
```

You can query all interactables:

```cpp
void AInteractionManager::QueryAllInteractables()
{
    UMActorQuerySubsystem* QuerySubsystem = UMActorQuerySubsystem::Get(GetWorld());
    
    FMActorQueryInput Query;
    Query.QueryTag = FGameplayTag::RequestGameplayTag(FName("Interactable"));
    Query.bIncludeChildrenTags = true;  // ← Include Door, Chest, NPC, etc.
    
    TArray<AActor*> Interactables;
    QuerySubsystem->QueryActors(Query, Interactables);
    
    // Now Interactables contains all actors tagged with:
    // Interactable, Interactable.Door, Interactable.Chest, Interactable.NPC, etc.
}
```

### Example 5: Multiple Tag Matching (All Tags Required)

Find actors that have BOTH a specific tag and are of a specific class:

```cpp
void AGameMode::FindEliteEnemies()
{
    UMActorQuerySubsystem* QuerySubsystem = UMActorQuerySubsystem::Get(GetWorld());
    
    FMActorQueryInput Query;
    Query.bUseTagContainer = true;
    Query.QueryTagContainer.AddTag(
        FGameplayTag::RequestGameplayTag(FName("NPC.Enemy"))
    );
    Query.QueryTagContainer.AddTag(
        FGameplayTag::RequestGameplayTag(FName("NPC.Elite"))
    );
    Query.QueryMatchType = EMQueryMatchType::All;  // Must have BOTH tags
    Query.bMatchActorClass = true;
    Query.ActorClassToMatch = ABossEnemy::StaticClass();
    
    TArray<AActor*> Enemies;
    QuerySubsystem->QueryActors(Query, Enemies);
}
```

### Example 6: Subscribe to Registration Events

```cpp
void AUIManager::SetupActorNotifications()
{
    if (UMActorQuerySubsystem* QuerySubsystem = UMActorQuerySubsystem::Get(GetWorld()))
    {
        // Listen for new queryable actors
        QuerySubsystem->OnQueryableActorRegisteredDelegate.AddDynamic(
            this, &AUIManager::OnActorRegistered
        );
        
        QuerySubsystem->OnQueryableActorUnregisteredDelegate.AddDynamic(
            this, &AUIManager::OnActorUnregistered
        );
    }
}

void AUIManager::OnActorRegistered(const FMOnQueryableActorRegisteredData& Data)
{
    if (const UMActorQueryComponent* QueryComponent = Data.ActorQueryComponent.Get())
    {
        FGameplayTag PrimaryTag = QueryComponent->GetPrimaryTag();
        UE_LOG(LogTemp, Warning, TEXT("Actor registered with tag: %s"), *PrimaryTag.ToString());
    }
}

void AUIManager::OnActorUnregistered(const FMOnQueryableActorRegisteredData& Data)
{
    if (const UMActorQueryComponent* QueryComponent = Data.ActorQueryComponent.Get())
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor unregistered"));
    }
}
```

### Example 7: Dynamic Tag Management

```cpp
void ACharacter::UpdateNPCStatus(ANPCCharacter* NPC, bool bNowIsFriendly)
{
    if (UMActorQueryComponent* QueryComponent = 
        NPC->FindComponentByClass<UMActorQueryComponent>())
    {
        if (bNowIsFriendly)
        {
            QueryComponent->RemoveTag(
                FGameplayTag::RequestGameplayTag(FName("NPC.Enemy"))
            );
            QueryComponent->AddTag(
                FGameplayTag::RequestGameplayTag(FName("NPC.Ally"))
            );
        }
        else
        {
            QueryComponent->RemoveTag(
                FGameplayTag::RequestGameplayTag(FName("NPC.Ally"))
            );
            QueryComponent->AddTag(
                FGameplayTag::RequestGameplayTag(FName("NPC.Enemy"))
            );
        }
    }
}
```

## When to Use MActorQuery

MActorQuery is ideal for scenarios where you need to reference dynamically spawned actors without maintaining hard references. Common use cases include:

- **Dynamic gameplay relationships**: Querying for newly spawned enemies, interactables, or NPCs
- **Decoupled gameplay systems**: Systems that need to find actors without knowing about them beforehand
- **Runtime actor discovery**: Finding actors that may or may not exist, whose lifetime you don't control
- **Level-based gameplay**: Discovering level-placed actors that are tagged with gameplay roles

### Considerations

- The registry uses a multi-map, so queries iterate through matching tags
- Hierarchical tag searching requires iterating the entire registry
- Frequent queries with class filtering will check each result's class
- Best suited for occasional lookups rather than per-frame queries on thousands of actors
- Not designed to replace spatial queries or line traces for proximity-based operations

## Integration with Other Systems

MActorQuery works well with other Unreal Engine systems for decoupled gameplay:

- **Gameplay Ability System (GAS)**: Query actors for ability-related roles or interactions
- **State Trees**: Look up target actors for state-driven behavior
- **Event/Delegate Systems**: Combine with actor registration events to trigger logic
- **Blueprint systems**: Use the queryable component to mark BP-driven actors

## Blueprint Nodes

The plugin provides these main Blueprint nodes:

- **Get Actor Query Subsystem**: Access the query subsystem
- **Query Actors**: Find multiple actors matching criteria
- **Query Actor**: Find single actor matching criteria
- **Get Actor Query Input (Actor For Tag)**: Helper to create single-actor query
- **Get Actor Query Input (Actors For Tag)**: Helper to create multi-actor query

## Troubleshooting

### Actors Not Being Found

1. Verify the actor has a `UMActorQueryComponent`
2. Check that tags are set correctly in the component
3. Ensure the component is registered (happens automatically in `BeginPlay()`)
4. Verify query parameters match your tag setup
5. Use `QueryInput.ToString()` to debug query parameters

### Editor World Queries

Actors are only queryable in the editor world during development. Runtime gameplay uses the game world subsystem.

### Dynamically Spawned Actors Not Appearing

- Ensure the spawned actor has `UMActorQueryComponent` added to it
- The component will auto-register during `BeginPlay()`
- If you need the actor queryable before `BeginPlay()`, call `RegisterQueryComponent()` manually
- When the actor is destroyed, the component automatically unregisters

