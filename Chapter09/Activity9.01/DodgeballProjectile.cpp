// Fill out your copyright notice in the Description page of Project Settings.


#include "DodgeballProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DodgeballCharacter.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ADodgeballProjectile::ADodgeballProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereComponent->SetSphereRadius(35.f);
	SphereComponent->SetCollisionProfileName(FName("Dodgeball"));
	SphereComponent->SetSimulatePhysics(true);
	//Simulation generates Hit events
	SphereComponent->SetNotifyRigidBodyCollision(true);
	// Listen to the OnComponentHit event by binding it to our function
	SphereComponent->OnComponentHit.AddDynamic(this, &ADodgeballProjectile::OnHit);

	// Set this Sphere Component as the root component,
	// otherwise collision won't behave properly
	RootComponent = SphereComponent;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->InitialSpeed = 1500.f;
}

void ADodgeballProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(5.f);
}

void ADodgeballProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADodgeballProjectile::OnHit(UPrimitiveComponent * HitComp,
								 AActor * OtherActor,
								 UPrimitiveComponent * OtherComp,
								 FVector NormalImpulse,
								 const FHitResult & Hit)
{
	if (BounceSound != nullptr && NormalImpulse.Size() > 600.0f)
	{
		UGameplayStatics::PlaySoundAtLocation(this,
											  BounceSound,
											  GetActorLocation(),
											  1.0f,
											  1.0f,
											  0.0f,
											  BounceSoundAttenuation);
	}
	ADodgeballCharacter* Player = Cast<ADodgeballCharacter>(OtherActor);
	if (Player != nullptr)
	{
		UHealthComponent* HealthComponent = Player->FindComponentByClass<UHealthComponent>();
		if (HealthComponent != nullptr)
		{
			HealthComponent->LoseHealth(Damage);
		}
		if (HitParticles != nullptr)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, GetActorTransform());
		}
		if (DamageSound != nullptr)
		{
			UGameplayStatics::PlaySound2D(this, DamageSound);
		}
		Destroy();
	}
}


