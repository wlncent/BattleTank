// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "TimerManager.h"
#include "Delegates/DelegateInstancesImpl.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Engine/EngineTypes.h"



// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionMesh = CreateDefaultSubobject <UStaticMeshComponent>(FName("Collision Mesh"));
	SetRootComponent(CollisionMesh);
	CollisionMesh->SetNotifyRigidBodyCollision(true);
	CollisionMesh->SetVisibility(false);

	LaunchBlast = CreateDefaultSubobject <UParticleSystemComponent>(FName("Launch Blast"));
	LaunchBlast->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	ImpactBlast = CreateDefaultSubobject <UParticleSystemComponent>(FName("Impact Blast"));
	ImpactBlast->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ImpactBlast->bAutoActivate = false;

	ProjectileMovement = CreateDefaultSubobject <UProjectileMovementComponent>(FName("Projectile Movement Component"));
	ProjectileMovement->bAutoActivate = false;

	ExplossionForce = CreateDefaultSubobject<URadialForceComponent>(FName("Explossion Force"));
	ExplossionForce->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	CollisionMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

}


void AProjectile::LaunchProjectile(float Speed)
{
	ProjectileMovement->SetVelocityInLocalSpace(FVector::ForwardVector * Speed);
	ProjectileMovement->Activate();
	 
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	
	LaunchBlast->Deactivate();
	ImpactBlast->Activate();
	ExplossionForce->FireImpulse();
	SetRootComponent(ImpactBlast);
	CollisionMesh->DestroyComponent();

	UGameplayStatics::ApplyRadialDamage(
		this,
		ProjectileDamage,
		GetActorLocation(),
		ExplossionForce->Radius,
		UDamageType::StaticClass(),
		TArray<AActor*>()
		);
	





	FTimerHandle  Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer,this,&AProjectile::OnTimerExpired,DestroyDelay,false);
}

void AProjectile::OnTimerExpired()
{
	Destroy();
}